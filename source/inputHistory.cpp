#include <cstdint>
#include <fstream>
#include <windows.h>

#include "gearLoader/gearLoader.hpp"
#include "baseMod/baseMod.hpp"
#include "frameStepping.h"

namespace {

// Offset from the executable base to the replay input row renderer.
constexpr uintptr_t NATIVE_INPUT_GROUP_DRAW_RVA = 0x002F2BA0;
// Offset from the executable base to the atlas cell drawing helper.
constexpr uintptr_t NATIVE_ATLAS_CELL_DRAW_RVA = 0x0010B800;
// Offset from the executable base to the input atlas sprite ID.
constexpr uintptr_t INPUT_ATLAS_ID_RVA = 0x006C6732;
// Offset to byte +6 of the first registry entry, used for a range check.
constexpr uintptr_t SPRITE_REGISTRY_RVA = 0x006DC152;
// Offset from the executable base to the button map used with FREE_PLAY.
constexpr uintptr_t BUTTON_MAPPINGS_1_RVA = 0x0069ED40;
// Offset from the executable base to the button map used without FREE_PLAY.
constexpr uintptr_t BUTTON_MAPPINGS_2_RVA = 0x006CE030;
// Number of registered sprite slots; larger IDs are not ready to draw.
constexpr uint16_t SPRITE_REGISTRY_LIMIT = 0x0AF0;
// Byte stride between consecutive sprite registry entries.
constexpr size_t SPRITE_REGISTRY_ENTRY_SIZE = 0x38;
// Settings file path relative to the game's working directory.
constexpr const char* CONFIG_PATH = "./mods/ACPR_InputHistory/settings.bin";
// Maximum number of consecutive input runs stored for each player.
constexpr size_t HISTORY_RUN_CAPACITY = 600;
// Maximum number of history rows drawn for each player.
constexpr size_t HISTORY_GROUP_CAPACITY = 21;
// Highest frame count shown by the native input history renderer.
constexpr uint32_t NATIVE_COUNT_MAX = 99;
// Vertical coordinate of the newest input history row.
constexpr int32_t HISTORY_FIRST_ROW_Y = 0x73;
// Vertical distance between input history rows.
constexpr int32_t HISTORY_ROW_SPACING = 0x0F;
// Input bits that encode directional movement.
constexpr uint32_t DIRECTION_MASK = 0x00F0;
// Default setting that shows the Training input history.
constexpr int32_t DEFAULT_DISPLAY = 1;
// Default sprite alpha: 255 is fully opaque.
constexpr int32_t DEFAULT_OPACITY = 255;
// Default draw depth, matching FrameMeter's base Z value.
constexpr int32_t DEFAULT_Z_POS = 70;
// Job mode value used while a battle is active.
constexpr ggxxacpr::JobMode BATTLE_JOB_MODE =
    static_cast<ggxxacpr::JobMode>(6);

struct ModContext {
    BaseMod::Api* baseModApi;
    int32_t display;
    int32_t opacity;
    int32_t zPos;
};

struct InputRun {
    uint32_t input = 0;
    uint32_t count = 0;
};

struct InputHistory {
    InputRun runs[HISTORY_RUN_CAPACITY]{};
    size_t nextWrite = 0;
    size_t count = 0;
};

// The native row renderer reads six 16-bit masks at DWORD offsets 0..0x14.
// Keep the 0x50-byte shape produced by the game's FUN_00541360 helper.
struct NativeInputMap {
    uint32_t values[20]{};
};
static_assert(sizeof(NativeInputMap) == 0x50);

// The game's atlas helper consumes this 0x30-byte sprite description in EAX.
struct NativeAtlasCell {
    int32_t spriteId;
    float x, y, z;
    float zoomX, zoomY;
    int32_t u0, v0, u1, v1;
    uint32_t alpha;
    uint32_t flags;
};
static_assert(sizeof(NativeAtlasCell) == 0x30);

// FUN_006F2BA0 draws one input history row. This mod calls it when Z POS is 0.
// Inputs (fastcall): ECX = held frame count, EDX = horizontal origin; the
// stack holds playerIndex (0 for P1, nonzero for P2), a pointer to inputMap
// (which supplies six button masks), the input bitmask, row Y, and alpha 0..1.
// Output: void; it submits sprites to the game's render pipeline. It clamps
// the count to 0..99, draws its digits, draws a direction when bits 0xF0 are
// set, then draws icons for the mapped buttons present in inputMask. P2 uses
// the mirrored row layout. The alpha is converted to sprite opacity.
using NativeInputGroupDraw = void (__attribute__((fastcall)) *)(
    int32_t runLength,
    int32_t horizontalOffset,
    int32_t playerIndex,
    const NativeInputMap* inputMap,
    uint32_t inputMask,
    int32_t y,
    float alpha);
// FUN_0050B800 draws one atlas cell. At nonzero Z POS, the mod builds these
// cells itself and passes a NativeAtlasCell pointer in EAX.
// Input: sprite ID, screen position and depth, zoom, atlas rectangle, alpha,
// and flags. Output: void; it submits a textured quad using the sprite
// registry's dimensions and the game's draw callback.
using NativeAtlasCellDraw = void (__attribute__((regparm(1))) *)(
    const NativeAtlasCell* cell);

ModContext modContext{nullptr, DEFAULT_DISPLAY, DEFAULT_OPACITY, DEFAULT_Z_POS};
InputHistory trainingHistory[2]{};
BaseMod::ModMenuEntry modMenuEntries[4]{};
bool trainingHistoryActive = false;

const NativeInputMap* CanonicalInputMap();
uint32_t InputFilterMask(const NativeInputMap* inputMap);

uint8_t* ImageBase() {
    return reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr));
}

void ClearTrainingHistory() {
    trainingHistory[0] = {};
    trainingHistory[1] = {};
    trainingHistoryActive = false;
}

void AppendInputRun(InputHistory& history, uint32_t input) {
    if (history.count > 0) {
        const size_t latest = (history.nextWrite + HISTORY_RUN_CAPACITY - 1) %
            HISTORY_RUN_CAPACITY;
        auto& lastRun = history.runs[latest];
        if (lastRun.input == input && lastRun.count < UINT32_MAX) {
            ++lastRun.count;
            return;
        }
    }

    history.runs[history.nextWrite] = {input, 1};
    history.nextWrite = (history.nextWrite + 1) % HISTORY_RUN_CAPACITY;
    if (history.count < HISTORY_RUN_CAPACITY) ++history.count;
}

const InputRun& RunAtAge(const InputHistory& history, size_t age) {
    const size_t latest = (history.nextWrite + HISTORY_RUN_CAPACITY - 1) % HISTORY_RUN_CAPACITY;
    const size_t index = (latest + HISTORY_RUN_CAPACITY - age) % HISTORY_RUN_CAPACITY;
    return history.runs[index];
}

const NativeInputMap* CanonicalInputMap() {
    // Fallback logical button masks and renderer metadata from the game's
    // FUN_00541360 helper; Training replaces the first six when its map is
    // readable.
    static constexpr NativeInputMap map{{
        0x8000, 0x4000, 0x1000, 0x2000, 0x0800, 0x0200,
        0x0001, 0x0008, 0x0000, 0x0400, 0x0100, 0x0000,
        0x0002, 0x0004, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x10001,
    }};
    return &map;
}

uint32_t InputFilterMask(const NativeInputMap* inputMap) {
    uint32_t mask = DIRECTION_MASK;
    if (!inputMap) return mask;
    for (size_t i = 0; i < 6; ++i) mask |= inputMap->values[i];
    return mask;
}

bool IsReadableAddress(const void* address, size_t length) {
    MEMORY_BASIC_INFORMATION info{};
    if (VirtualQuery(address, &info, sizeof(info)) != sizeof(info)) return false;
    if (info.State != MEM_COMMIT ||
            (info.Protect & (PAGE_NOACCESS | PAGE_GUARD)) != 0) return false;
    const auto regionStart = reinterpret_cast<uintptr_t>(info.BaseAddress);
    const auto regionEnd = regionStart + info.RegionSize;
    const auto target = reinterpret_cast<uintptr_t>(address);
    return target >= regionStart && target <= regionEnd &&
        length <= regionEnd - target;
}

uintptr_t ActiveButtonMappingRva(uint32_t gameModeFlags) {
    // Match GearLoader's mod menu selection according to FREE_PLAY.
    return (gameModeFlags & GAME_MODE_FEATURE_FLAGS_FREE_PLAY) != 0
        ? BUTTON_MAPPINGS_1_RVA
        : BUTTON_MAPPINGS_2_RVA;
}

void ApplyActiveButtonMapping(NativeInputMap& inputMap, uint32_t gameModeFlags) {
    // The first six DWORDs are Punch, Kick, Slash, Heavy Slash, Dust, Taunt.
    const uintptr_t mappingRva = ActiveButtonMappingRva(gameModeFlags);
    const auto* mapping = reinterpret_cast<const uint32_t*>(ImageBase() + mappingRva);
    if (!IsReadableAddress(mapping, 6 * sizeof(uint32_t))) return;

    for (size_t button = 0; button < 6; ++button) {
        inputMap.values[button] = mapping[button];
    }
}

bool TrainingResetPressed(
    const GGXXACPR_PlayerInput* input, uint32_t gameModeFlags) {
    // GearLoader's ButtonMapping slot 6 is the configurable PosResetKey.
    const auto* resetMask = reinterpret_cast<const uint32_t*>(
        ImageBase() + ActiveButtonMappingRva(gameModeFlags) +
        6 * sizeof(uint32_t));
    if (!IsReadableAddress(resetMask, sizeof(*resetMask)) || *resetMask == 0) {
        return false;
    }
    const uint32_t pressed = input[0].InputPress1 | input[0].InputPress2 |
        input[1].InputPress1 | input[1].InputPress2;
    return (pressed & *resetMask) != 0;
}

bool NativeSpriteAtlasReady(uint16_t* spriteIdOut) {
    auto* base = ImageBase();
    auto* spriteIdAddress = reinterpret_cast<volatile uint16_t*>(
        base + INPUT_ATLAS_ID_RVA);
    const uint16_t spriteId = *spriteIdAddress;
    if (spriteIdOut) *spriteIdOut = spriteId;

    // FUN_005096F0 initializes 0xAF0 registry slots. The EXE's static atlas
    // ID sentinel is 0x31B8, outside that range.
    if (spriteId >= SPRITE_REGISTRY_LIMIT) return false;

    const uintptr_t entryAddress = reinterpret_cast<uintptr_t>(base) +
        SPRITE_REGISTRY_RVA + static_cast<uintptr_t>(spriteId) * SPRITE_REGISTRY_ENTRY_SIZE;
    return IsReadableAddress(reinterpret_cast<const void*>(entryAddress), 4);
}

void DrawAtlasCell(
    NativeAtlasCellDraw drawCell,
    uint16_t spriteId,
    int32_t x,
    int32_t y,
    int32_t z,
    uint32_t opacity,
    int32_t u0,
    int32_t v0,
    int32_t u1,
    int32_t v1) {
    NativeAtlasCell cell{};
    cell.spriteId = spriteId;
    cell.x = static_cast<float>(x);
    cell.y = static_cast<float>(y);
    cell.z = static_cast<float>(z);
    cell.zoomX = 1.0f;
    cell.zoomY = 1.0f;
    cell.u0 = u0;
    cell.v0 = v0;
    cell.u1 = u1;
    cell.v1 = v1;
    cell.alpha = opacity;
    drawCell(&cell);
}

int32_t DirectionCellIndex(uint32_t input) {
    if (input & 0x80) {
        if (input & 0x10) return 4;
        return (input & 0x40) ? 6 : 0;
    }
    if (input & 0x20) {
        if (input & 0x10) return 5;
        return (input & 0x40) ? 7 : 1;
    }
    if (input & 0x40) return 3;
    return (input & 0x10) ? 2 : -1;
}

void DrawRowAtZ(
    NativeAtlasCellDraw drawCell,
    uint16_t spriteId,
    uint32_t runLength,
    int32_t horizontalOffset,
    int32_t player,
    const NativeInputMap& inputMap,
    uint32_t input,
    int32_t y,
    uint32_t opacity,
    int32_t z) {
    const int32_t digitX = horizontalOffset - (player == 0 ? 0 : 14);
    const uint32_t tens = runLength / 10;
    const uint32_t ones = runLength % 10;
    int32_t digitOffset = 0;
    if (tens > 0) {
        const int32_t u0 = static_cast<int32_t>(tens) * 8;
        DrawAtlasCell(drawCell, spriteId, digitX, y + 1, z, opacity,
            u0, 27, u0 + 7, 38);
        digitOffset = 7;
    }
    const int32_t onesU0 = static_cast<int32_t>(ones) * 8;
    DrawAtlasCell(drawCell, spriteId, digitX + digitOffset, y + 1, z,
        opacity, onesU0, 27, onesU0 + 7, 38);

    const bool hasDirection = (input & DIRECTION_MASK) != 0;
    if (hasDirection) {
        const int32_t direction = DirectionCellIndex(input);
        if (direction >= 0) {
            const int32_t directionX = horizontalOffset +
                (player == 0 ? 21 : -33);
            const int32_t u0 = direction * 12;
            DrawAtlasCell(drawCell, spriteId, directionX, y + 1, z,
                opacity, u0, 0, u0 + 11, 11);
        }
    }

    const int32_t buttonGap = hasDirection ? 33 : 21;
    int32_t buttonX = player == 0
        ? horizontalOffset + buttonGap
        : horizontalOffset - buttonGap - 10;
    for (int32_t position = 0; position < 6; ++position) {
        const int32_t button = player == 0 ? position : 5 - position;
        if ((input & static_cast<uint16_t>(inputMap.values[button])) == 0) {
            continue;
        }
        const int32_t u0 = button * 10;
        DrawAtlasCell(drawCell, spriteId, buttonX, y, z, opacity,
            u0, 12, u0 + 9, 26);
        buttonX += player == 0 ? 10 : -10;
    }
}

void DrawNativeHistory(
    const NativeInputMap (&inputMaps)[2], uint32_t opacity, int32_t z) {
    if (opacity == 0) return;
    uint16_t spriteId = 0;
    if (!NativeSpriteAtlasReady(&spriteId)) {
        return;
    }

    auto* base = ImageBase();
    const auto drawGroup = reinterpret_cast<NativeInputGroupDraw>(
        base + NATIVE_INPUT_GROUP_DRAW_RVA);
    const auto drawCell = reinterpret_cast<NativeAtlasCellDraw>(
        base + NATIVE_ATLAS_CELL_DRAW_RVA);

    for (int32_t player = 0; player < 2; ++player) {
        const NativeInputMap* inputMap = &inputMaps[player];
        const auto& history = trainingHistory[player];
        size_t row = 0;
        const int32_t horizontalOffset = player == 0 ? 5 : 0x276 + 5;
        for (size_t age = 0; age < history.count && row < HISTORY_GROUP_CAPACITY; ++age) {
            const auto& run = RunAtAge(history, age);
            const uint32_t displayedCount = run.count > NATIVE_COUNT_MAX
                ? NATIVE_COUNT_MAX
                : run.count;
            const int32_t y = HISTORY_FIRST_ROW_Y +
                static_cast<int32_t>(row) * HISTORY_ROW_SPACING;
            // The game draws the whole row at Z 0; custom Z uses its atlas
            // helper for each cell in the same row layout.
            if (z == 0) {
                drawGroup(
                    static_cast<int32_t>(displayedCount),
                    horizontalOffset,
                    player,
                    inputMap,
                    run.input,
                    y,
                    static_cast<float>(opacity) / 255.0f);
            } else {
                DrawRowAtZ(drawCell, spriteId, displayedCount,
                    horizontalOffset, player, *inputMap, run.input, y,
                    opacity, z);
            }
            ++row;
        }
    }
}

void SaveSettings() {
    std::ofstream file(CONFIG_PATH, std::ios::binary | std::ios::trunc);
    if (!file) return;
    file.write(reinterpret_cast<const char*>(&modContext.display),
        sizeof(modContext.display));
    file.write(reinterpret_cast<const char*>(&modContext.opacity),
        sizeof(modContext.opacity));
    file.write(reinterpret_cast<const char*>(&modContext.zPos),
        sizeof(modContext.zPos));
}

void LoadSettings() {
    std::ifstream file(CONFIG_PATH, std::ios::binary);
    int32_t display = DEFAULT_DISPLAY;
    if (file.read(reinterpret_cast<char*>(&display), sizeof(display)) &&
            (display == 0 || display == 1)) {
        modContext.display = display;
    }
    // Existing settings.bin files contain only the former ENABLE value.
    int32_t opacity = DEFAULT_OPACITY;
    if (file.read(reinterpret_cast<char*>(&opacity), sizeof(opacity)) &&
            opacity >= 0 && opacity <= 255) {
        modContext.opacity = opacity;
    }
    int32_t zPos = DEFAULT_Z_POS;
    if (file.read(reinterpret_cast<char*>(&zPos), sizeof(zPos)) &&
            zPos >= 0 && zPos <= 800) {
        modContext.zPos = zPos;
    }
}

void BASEMOD_CALL OnDisplayChanged(int32_t display) {
    modContext.display = display != 0;
    SaveSettings();
}

void BASEMOD_CALL OnOpacityChanged(int32_t opacity) {
    modContext.opacity = opacity < 0 ? 0 : opacity > 255 ? 255 : opacity;
    SaveSettings();
}

void BASEMOD_CALL OnZPosChanged(int32_t zPos) {
    modContext.zPos = zPos < 0 ? 0 : zPos > 800 ? 800 : zPos;
    SaveSettings();
}

void BASEMOD_CALL RestoreDefaults() {
    modContext.display = DEFAULT_DISPLAY;
    modContext.opacity = DEFAULT_OPACITY;
    modContext.zPos = DEFAULT_Z_POS;
    SaveSettings();
}

void RegisterModMenu(BaseMod::Api* api) {
    static const char* onOffLabels[] = {"OFF", "ON"};
    modMenuEntries[0] = {
        "DISPLAY",
        &modContext.display,
        0,
        1,
        onOffLabels,
        nullptr,
        OnDisplayChanged,
    };
    modMenuEntries[1] = {
        "OPACITY",
        &modContext.opacity,
        0,
        255,
        nullptr,
        nullptr,
        OnOpacityChanged,
    };
    modMenuEntries[2] = {
        "Z POS",
        &modContext.zPos,
        0,
        800,
        nullptr,
        nullptr,
        OnZPosChanged,
    };
    modMenuEntries[3] = {
        "DEFAULT",
        nullptr,
        0,
        0,
        nullptr,
        RestoreDefaults,
        nullptr,
    };
    api->ModMenu.RegisterMenuTab("INPUT HISTORY", modMenuEntries, 4);
}

bool IsInputHistoryModeActive(BaseMod::Api* api) {
    auto& game = api->GameData;
    // The online menu value can contain upper-byte state (0x01010003 was
    // observed in Online Training); its low byte still identifies ONLINE.
    const uint32_t menuItem =
        static_cast<uint32_t>(game.GetMainMenuSelection()) & 0xFF;
    const bool battleActive = game.IsInGame() &&
        game.GetJobMode() == BATTLE_JOB_MODE;
    const uint32_t gameModeFlags =
        static_cast<uint32_t>(game.GetGameModeFeatureFlags());
    const bool trainingMenu =
        menuItem == static_cast<uint32_t>(ggxxacpr::MenuItem::TRAINING) ||
        menuItem == static_cast<uint32_t>(ggxxacpr::MenuItem::ONLINE);

    return battleActive && trainingMenu &&
        (gameModeFlags & GAME_MODE_FEATURE_FLAGS_TRAINING) != 0;
}

void BASEMOD_CALL BeforeGameUpdate(
    ModContext* context,
    const BaseMod_HookContext*,
    const BaseMod_GameUpdateInfo*) {
    if (IsInputHistoryModeActive(context->baseModApi)) {
        HandleFrameStepping_PreUpdate(context->baseModApi);
    }
}

void BASEMOD_CALL AfterGameUpdate(
    ModContext* context,
    const BaseMod_HookContext*,
    const BaseMod_GameUpdateInfo*) {

    auto& game = context->baseModApi->GameData;
    const bool trainingBattle = IsInputHistoryModeActive(context->baseModApi);
    const uint32_t gameModeFlags =
        static_cast<uint32_t>(game.GetGameModeFeatureFlags());

    if (!trainingBattle) {
        HandleFrameStepping_PostUpdate(context->baseModApi);
        ClearTrainingHistory();
        return;
    }

    NativeInputMap inputMaps[2] = {*CanonicalInputMap(), *CanonicalInputMap()};
    // Both players' observed action icons follow the first active table entry.
    ApplyActiveButtonMapping(inputMaps[0], gameModeFlags);
    inputMaps[1] = inputMaps[0];
    auto* input = game.GetPlayerInputStructArr();

    if (!trainingHistoryActive) {
        ClearTrainingHistory();
        trainingHistoryActive = true;
    }

    if (game.GetPauseState() == 0 && input) {
        if (TrainingResetPressed(input, gameModeFlags)) {
            ClearTrainingHistory();
            trainingHistoryActive = true;
        } else {
            // Raw1 is current accepted state. Raw2 is the previous state
            // and would keep a released input visible for one extra row.
            AppendInputRun(trainingHistory[0],
                input[0].InputRaw1 & InputFilterMask(&inputMaps[0]));
            AppendInputRun(trainingHistory[1],
                input[1].InputRaw1 & InputFilterMask(&inputMaps[1]));
        }
    }

    // Sample the stepped update before restoring the raw input and re-pausing.
    HandleFrameStepping_PostUpdate(context->baseModApi);

    if (context->display) {
        DrawNativeHistory(inputMaps, context->opacity, context->zPos);
    }
}

} // namespace

GEARLOADER_EXPORT void GEARLOADER_CALL Init(
    GearLoaderContext* loaderContext,
    GearLoaderApi* loaderApi) {

    GearLoader::Api gearLoader(loaderApi, loaderContext);
    const BaseMod_Api* baseModCApi = nullptr;
    SemanticVersion baseModVersion{};
    const int result = gearLoader.RetrieveModApi<BaseMod_Api>(
        BASEMOD_NAME,
        BASEMOD_API_VERSION,
        &baseModCApi,
        &baseModVersion);

    if (result != 0 || !baseModCApi) {
        gearLoader.Log(GearLoader::LogLevel::ERR, "BaseMod API was not found");
        return;
    }

    LoadSettings();
    modContext.baseModApi = new BaseMod::Api(baseModCApi);
    modContext.baseModApi->Hooks.BeforeGameUpdate<ModContext>(
        BeforeGameUpdate,
        &modContext);
    modContext.baseModApi->Hooks.AfterGameUpdate<ModContext>(
        AfterGameUpdate,
        &modContext);
    RegisterModMenu(modContext.baseModApi);

    gearLoader.Log(GearLoader::LogLevel::INFO,
        "Training input history renderer initialized");
}
