#include "frameStepping.h"

#include <cstddef>
#include <cstdint>
#include <windows.h>

namespace {

constexpr uintptr_t BUTTON_MAPPINGS_FREE_PLAY_RVA = 0x0069ED40;
constexpr uintptr_t BUTTON_MAPPINGS_DEFAULT_RVA = 0x006CE030;
constexpr size_t BUTTON_MAPPING_ROW_SIZE = 0x50;
constexpr size_t BUTTON_MAPPING_ENEMY_WALK_INDEX = 12;
constexpr size_t BUTTON_MAPPING_ATTACK_COUNT = 6;
constexpr uint32_t DIRECTION_MASK = INPUT_UP | INPUT_RIGHT | INPUT_DOWN | INPUT_LEFT;

bool repause = false;
uint32_t pendingRawRestoreMask[2]{};
uint32_t pendingRawRestoreBits[2]{};

bool IsReadableAddress(const void* address, size_t length) {
    MEMORY_BASIC_INFORMATION info{};
    if (VirtualQuery(address, &info, sizeof(info)) != sizeof(info)) return false;
    if (info.State != MEM_COMMIT ||
            (info.Protect & (PAGE_NOACCESS | PAGE_GUARD)) != 0) return false;

    const uintptr_t regionStart = reinterpret_cast<uintptr_t>(info.BaseAddress);
    const uintptr_t regionEnd = regionStart + info.RegionSize;
    const uintptr_t target = reinterpret_cast<uintptr_t>(address);
    return target >= regionStart && target <= regionEnd &&
        length <= regionEnd - target;
}

bool ReadButtonMappingRow(
    BaseMod::GameDataApi& game,
    size_t playerIndex,
    const uint32_t*& row
) {
    const uintptr_t imageBase = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
    if (imageBase == 0) return false;

    const uint32_t modeFlags = static_cast<uint32_t>(game.GetGameModeFeatureFlags());
    const uintptr_t tableRva = (modeFlags & GAME_MODE_FEATURE_FLAGS_FREE_PLAY) != 0
        ? BUTTON_MAPPINGS_FREE_PLAY_RVA
        : BUTTON_MAPPINGS_DEFAULT_RVA;
    const uintptr_t address = imageBase + tableRva + playerIndex * BUTTON_MAPPING_ROW_SIZE;
    const auto* candidate = reinterpret_cast<const uint32_t*>(address);
    if (!IsReadableAddress(candidate, BUTTON_MAPPING_ROW_SIZE)) return false;

    row = candidate;
    return true;
}

uint32_t EnemyWalkConflictMask(const uint32_t* mappingRow, uint32_t enemyWalkMask) {
    uint32_t conflicts = enemyWalkMask & DIRECTION_MASK;
    for (size_t button = 0; button < BUTTON_MAPPING_ATTACK_COUNT; ++button) {
        conflicts |= enemyWalkMask & mappingRow[button];
    }
    return conflicts;
}

} // namespace

void HandleFrameStepping_PreUpdate(BaseMod::Api* api) {
    auto& game = api->GameData;
    pendingRawRestoreMask[0] = pendingRawRestoreMask[1] = 0;
    pendingRawRestoreBits[0] = pendingRawRestoreBits[1] = 0;

    const uint32_t modeFlags = static_cast<uint32_t>(game.GetGameModeFeatureFlags());
    if ((modeFlags & GAME_MODE_FEATURE_FLAGS_TRAINING) == 0) {
        return;
    }

    auto* inputs = game.GetPlayerInputStructArr();
    if (!inputs) return;

    uint32_t pressedMasks[2]{};
    const uint32_t* mappingRows[2]{};
    bool hasTrigger = false;

    // InputPress2 is the game's fresh-press edge. Use it as the click signal;
    // the post-update raw-state restore below prevents a held key from
    // generating another edge after its first masked update.
    for (size_t playerIndex = 0; playerIndex < 2; ++playerIndex) {
        if (!ReadButtonMappingRow(game, playerIndex, mappingRows[playerIndex])) {
            continue;
        }

        const uint32_t enemyWalkMask =
            mappingRows[playerIndex][BUTTON_MAPPING_ENEMY_WALK_INDEX];
        const bool isDown = enemyWalkMask != 0 &&
            (inputs[playerIndex].InputRaw1 & enemyWalkMask) != 0;
        const bool justPressed = isDown &&
            (inputs[playerIndex].InputPress2 & enemyWalkMask) != 0;

        if (justPressed) {
            pressedMasks[playerIndex] = enemyWalkMask;
            hasTrigger = true;
        }
    }

    if (!hasTrigger || game.GetPauseState() != 2 || game.GetPauseDisplayState() != 0) {
        return;
    }

    // Refuse ambiguous mappings: stripping EnemyWalk would also strip Up,
    // P/K/S/HS/D/Taunt when any of those controls share a bit.
    for (size_t playerIndex = 0; playerIndex < 2; ++playerIndex) {
        const uint32_t enemyWalkMask = pressedMasks[playerIndex];
        if (enemyWalkMask == 0) continue;

        if (!mappingRows[playerIndex]) return;
        const uint32_t conflicts =
            EnemyWalkConflictMask(mappingRows[playerIndex], enemyWalkMask);
        if (conflicts != 0) {
            return;
        }
    }

    // Hide EnemyWalk from the one native update, but remember the accepted raw
    // state so it can be restored afterward. InputRaw1 is also the prior state
    // used by the game to derive the next press edge; leaving EnemyWalk cleared
    // would make a physically held key look freshly pressed every update.
    for (size_t playerIndex = 0; playerIndex < 2; ++playerIndex) {
        const uint32_t enemyWalkMask = pressedMasks[playerIndex];
        if (enemyWalkMask == 0) continue;

        const uint32_t raw1Before = inputs[playerIndex].InputRaw1;
        const uint32_t press2Before = inputs[playerIndex].InputPress2;
        pendingRawRestoreMask[playerIndex] = enemyWalkMask;
        pendingRawRestoreBits[playerIndex] = raw1Before & enemyWalkMask;
        const uint32_t raw1After = raw1Before & ~enemyWalkMask;
        const uint32_t press2After = press2Before & ~enemyWalkMask;
        inputs[playerIndex].InputRaw1 = raw1After;
        inputs[playerIndex].InputPress2 = press2After;
    }

    *game.GetCApi()->GetPauseState() = 0;
    repause = true;
}

void HandleFrameStepping_PostUpdate(BaseMod::Api* api) {
    if (!repause) return;

    auto* inputs = api->GameData.GetPlayerInputStructArr();
    if (inputs) {
        for (size_t playerIndex = 0; playerIndex < 2; ++playerIndex) {
            const uint32_t mask = pendingRawRestoreMask[playerIndex];
            if (mask == 0) continue;

            const uint32_t raw1Before = inputs[playerIndex].InputRaw1;
            const uint32_t raw1After =
                (raw1Before & ~mask) | pendingRawRestoreBits[playerIndex];
            inputs[playerIndex].InputRaw1 = raw1After;
        }
    }

    if (api->GameData.GetPauseState() == 0) {
        *api->GameData.GetCApi()->GetPauseState() = 2;
    }
    pendingRawRestoreMask[0] = pendingRawRestoreMask[1] = 0;
    pendingRawRestoreBits[0] = pendingRawRestoreBits[1] = 0;
    repause = false;
}
