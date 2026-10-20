#ifndef BASEMOD_HPP
#define BASEMOD_HPP

#include "baseMod_c.h"
#include <string>
#include "gearLoader/ggxxacpr.hpp"

namespace BaseMod {

using HookId = BaseMod_HookId;
using HookContext = BaseMod_HookContext;
using PeakMessageArgs = BaseMod_PeekMessageArgs;
using PeekMessageInfo = BaseMod_PeekMessageInfo;
using GameUpdateInfo = BaseMod_GameUpdateInfo;
using DrawInfo = BaseMod_DrawInfo;
using SaveGameInfo = BaseMod_SaveGameInfo;

enum class PushboxDimensionArrayType : int32_t {
    STANDING_WIDTH = BM_PD_STANDING_WIDTH,
    STANDING_HEIGHT_AC = BM_PD_STANDING_HEIGHT_AC,
    STANDING_HEIGHT_PR = BM_PD_STANDING_HEIGHT_PR,
    CROUCHING_WIDTH = BM_PD_CROUCHING_WIDTH,
    CROUCHING_HEIGHT = BM_PD_CROUCHING_HEIGHT,
    AIRBORNE_WIDTH = BM_PD_AIRBORNE_WIDTH,
    AIRBORNE_HEIGHT = BM_PD_AIRBORNE_HEIGHT,
};
enum class ThrowRangeArrayType : int32_t {
    GROUND_AC = BM_TR_GROUND_AC,
    GROUND_PR = BM_TR_GROUND_PR,
    AIR_HORIZONTAL_AC = BM_TR_AIR_HORIZONTAL_AC,
    AIR_HORIZONTAL_PR = BM_TR_AIR_HORIZONTAL_PR,
    AIR_UPPER = BM_TR_AIR_UPPER,    // This array is not split between Accent Core and Plus R
    AIR_LOWER = BM_TR_AIR_LOWER,    // This array is not split between Accent Core and Plus R
};
enum class DrawArrowSpriteDirection : uint32_t {
    LEFT = BM_DASD_LEFT,
    RIGHT = BM_DASD_RIGHT,
    UP = BM_DASD_UP,
    DOWN = BM_DASD_DOWN,
    UP_LEFT = BM_DASD_UP_LEFT,
    UP_RIGHT = BM_DASD_UP_RIGHT,
    DOWN_LEFT = BM_DASD_DOWN_LEFT,
    DOWN_RIGHT = BM_DASD_DOWN_RIGHT,
};
enum class DrawScrollArrowFlags : int32_t {
    NONE = BM_DSAF_NONE,
    UP = BM_DSAF_UP,
    DOWN = BM_DSAF_DOWN,
    BOTH = BM_DSAF_BOTH,
};

// Generic functionality for wrapper classes
template<typename T>
class ApiWrapper {
public:
    ApiWrapper() : _ref(nullptr) {}
    ApiWrapper(const T* ref) : _ref(ref) {}
    bool VersionError() {
        return (BASEMOD_API_VERSION_NUM & 0xFF0000) !=
            (reinterpret_cast<CApiBase*>(_ref)->version & 0xFF0000);
    }
    bool IsValid() { return _ref != nullptr; }
    const T* GetCApi() { return _ref; }
protected:
    const T* _ref;
private:
    // All Base Mod C-APIs should fit this format
    struct CApiBase {
        uint32_t size;
        uint32_t version;
    };
};

class NativeFunctionsApi : public ApiWrapper<BaseMod_NativeFunctionsApi> {
public:
    /**
     *  \brief Draws text to the screen during battle using the game's text glyph system.
     * 
     *  This function must be called before the game begins its drawing process.
     *  It is recommend to call this function in the `AfterGameUpdate` hook.
     * 
     *  \image html RenderCockpitFontTextDemo.png "Example input/output. Font type depends on game setting."
     * 
     *  \param text A pointer to the text string to be displayed. The string format is a subset of ASCII.
     *      The character set is limited to upper case letters, numbers, and the following special symbols: "-+.!?/():&"
     *      The following characters map to additional special characters:
     *          ",~acegiwxyz" maps to "•±on•utabyx".
     *          '>' = END character from the highscore initials screen.
     *          "bdfh" = down/left/right/up arrows
     *  \param x Internal resolution screen-space coordinate (640x480). Left edge is 0, right is 640.
     *  \param y Internal resolution screen-space coordinate (640x480). Top edge is 0, bottom is 480.
     *  \param z The draw order/depth buffer value. Lower values draw later / appear in front of other text and sprites.
     *  \param alpha Transparency value [0-255].
     *  \param size Scaling value, standard size is 1.0f which results in a text glyph of size 12x15px (internal resolution).
     */
    void RenderCockpitFontText(std::string text, int32_t x, int32_t y, float z, uint8_t alpha, float size) {
        _ref->RenderCockpitFontText(text.c_str(), x, y, z, alpha, size);
    }

    /**
     *  \brief Draws text to the screen such as the text seen in pause menus.
     * 
     *  This function must be called before the game begins its drawing process.
     *  It is recommend to call this function in the `AfterGameUpdate` hook.
     *  Max string size is 512 characters. This function can handle '\n' characters.
     * 
     *  \param text A pointer to the text string to be displayed.
     *  \param x Internal resolution screen-space coordinate (640x480). Left edge is 0, right is 640.
     *  \param y Internal resolution screen-space coordinate (640x480). Top edge is 0, bottom is 480.
     *  \param z The draw order/depth buffer value. Lower values draw later / appear in front of other text and sprites.
     *  \param alpha Transparency value from 0 to 1, 1 being fully opaque.
     *  \param animCounter Optional pointer to an animation counter. If defined, the text will perform a 'typewriter' animation
     *      drawing an additional letter at a time until fully rendered. One letter will be rendered per 128 units on this counter.
     *      When the animation is complete, this value will be set to -1 and will no longer increment.
     *  \param animSpeed The amount to increment the `animCounter` if it is defined. Set to 128 for 1 letter per frame.
     *  \param ignoreSpriteMask If true, ignores color and alpha params and draws as fully opaque white text.
     *  \param color The color of the text string (0xRRGGBB)
     */
    void RenderMenuText(const char* text, int32_t x, int32_t y, float z, float alpha,
        int32_t* animCounter, int32_t animSpeed, bool ignoreSpriteMask, uint32_t color
    ) {
        _ref->RenderMenuText(text, x, y, z, alpha, animCounter, animSpeed, ignoreSpriteMask, color);
    }
    /**
     *  \brief Draws centered aligned text to the screen.
     * 
     *  A wrapper of `RenderMenuText` that draws the text centered around the `x` parameter.
     *  `y` position is unchanged. This function handles multi-line strings but centers based
     *  on the longest line only (i.e. each line is NOT individually centered).
     * 
     *  \param text A pointer to the text string to be displayed.
     *  \param x Internal resolution screen-space coordinate (640x480). Left edge is 0, right is 640.
     *  \param y Internal resolution screen-space coordinate (640x480). Top edge is 0, bottom is 480.
     *  \param z The draw order/depth buffer value. Lower values draw later / appear in front of other text and sprites.
     *  \param alpha Transparency value from 0 to 1, 1 being fully opaque.
     *  \param color The color of the text string (0xRRGGBB)
     *  \param ignoreSpriteMask If true, ignores color and alpha params and draws as fully opaque white text.
     */
    void RenderMenuTextCenterAligned(const char* text, int32_t x, int32_t y,
        float z, float alpha, uint32_t color, bool ignoreSpriteMask
    ) {
        _ref->RenderMenuTextCenterAligned(text, x, y, z, alpha, color, ignoreSpriteMask);
    }
    /**
     *  \brief A higher level text rendering function that triggers a text popup animation in-game (e.g. COUNTER HIT / RECOVERY).
     * 
     *  \param playerIndex Which side of the screen to display the pop up.
     *  \param text The text to be displayed. See `RenderCockpitFontText` for format and available characters.
     */
    void RenderPopUpText(int32_t playerIndex, std::string text) {
        _ref->RenderPopUpText(playerIndex, text.c_str());
    }

    /**
     *  \brief Plays a sound effect.
     * 
     *  An internal array is indexed with the `id` parameter without validity checks.
     *  Invalid values can cause invalid memory access.
     * 
     *  \param id The given id maps to the "COMMON SE" sound effect in the Sound menu.
     *      See `github.com/youknow232/gearloader/tree/main/docs/SoundEffectIdMap.txt` for the id mappings.
     *  \return true if `id` param is zero
     */
    bool PlayCommonSoundEffect(uint32_t id) {
        return _ref->PlayCommonSoundEffect(id);
    }

    /**
     *  \brief Draws a sprite to the screen.
     * 
     *  See `GGXXACPR_DrawSpriteParams`
     * 
     *  \param params Combined parameter struct
     *  \param ignoreSpriteMask A 4-byte boolean value. If a non-zero value is passed, ignores
     *      color and alpha params.
     */
    void DrawSprite(GGXXACPR_DrawSpriteParams* params, bool ignoreSpriteMask) {
        _ref->DrawSprite(params, ignoreSpriteMask ? 1 : 0);
    }

    /**
     *  \brief `DrawSprite` wrapper function that draws an arrow sprite.
     * 
     *  \param directionFlag see enum `BM_DrawArrowSpriteDirection`
     *  \param x Internal resolution screen-space coordinate (640x480). Left edge is 0, right is 640.
     *  \param y Internal resolution screen-space coordinate (640x480). Top edge is 0, bottom is 480.
     *  \param z The draw order/depth buffer value. Lower values draw later / appear in front of other text and sprites.
     *  \param alpha Transparency value from 1 to 255, 1 being fully opaque.
     */
    void DrawArrowSprite(DrawArrowSpriteDirection direction, int32_t x, int32_t y, int32_t z, uint32_t alpha) {
        _ref->DrawArrowSprite(static_cast<uint32_t>(direction), x, y, z, alpha);
    }

    /**
     *  \brief Draws a triangle strip primitive.
     * 
     *  The coordinates of the vertices are given in internal resolution screen space coordinates.
     *  The top left pixel is (0, 0) and the bottom right pixel is (640, 480).
     * 
     *  \param vertices The vertices making up the triangle strip.
     *  \param numVertices The number of vertices.
     */
    void DrawTriStrip(ggxxacpr::ColorVertex* vertices, uint32_t numVertices) {
        _ref->DrawTriStrip(vertices, numVertices);
    }

    /**
     *  \brief Draws a quad to the screen.
     * 
     *  A simple helper function that calls constructs four vertices from the given
     *  parameters and passes them to `DrawTriStrip`. The edges of the quad are
     *  given in internal resolution screen space coordinates. The top left pixel
     *  is (0, 0) and the bottom right pixel is (640, 480).
     * 
     *  \param left edge position
     *  \param top edge position
     *  \param right edge position
     *  \param bottom edge position
     *  \param z The Z coordinate for each vertex
     *  \param color ARGB color value (i.e. `D3DCOLOR`)
     */
    void DrawQuad(int32_t left, int32_t top, int32_t right, int32_t bottom, int32_t z, uint32_t color) {
        _ref->DrawQuad(left, top, right, bottom, z, color);
    }
    /**
     *  \brief Registers sprite data to a spriteId to later be rendered with `BaseMod_NativeFunctionsApi::DrawSprite`.
     * 
     *  When registering multiple sprites, each sprite will be sequentially
     *  registered with spriteIds starting with the given `spriteId` parameter.
     *  Example: Registering 4 sprites at spriteId 100 registers ids 100,101,102,103.
     * 
     *  See tutorial on registering and using
     *  [custom sprites](https://YouKnow232.github.io/GearLoader/doxygen/md_docs_2_custom_sprites_tutorial)
     *  for more information.
     * 
     *  \param spriteId The slot sprite data will be loaded into
     *  \param textureDataArray The raw sprite data.
     *  \param count The number of sprites to register. If 0, sprites will be registered until
     *      a null pointer is found in the given array.
     *  \return The number of sprites registered
     */
    void RegisterSprites(uint32_t spriteId, void* textureDataArray, uint32_t count) {
        _ref->RegisterSprites(spriteId, textureDataArray, count);
    }

    /**
     *  \brief If the given player currently has an active command grab,
     *      returns its command grab id, else return 0.
     */
    uint32_t GetActiveCommandGrabId(ggxxacpr::Player player) {
        return _ref->GetActiveCommandGrabId(player.getRaw());
    }
};

class CharDataApi : public ApiWrapper<BaseMod_CharDataApi> {
public:
    /**
     *  \brief Returns a pointer to the push box array specified by the `type` parameter.
     * 
     *  These arrays are all indexed with an `ggxxacpr::EntityId` value as returned by `ggxxacpr::Player::id`.
     * 
     *  \param type The type of array to get
     */
    uint16_t* GetPushboxDimensionArray(PushboxDimensionArrayType type) {
        return _ref->GetPushboxDimensionArray(static_cast<int32_t>(type));
    }
    /**
     *  \brief Returns a pointer to the game's airborne pushbox offset array.
     * 
     *  These values are subtracted from the player's y position when calculating airborne pushbox collisions.
     *  There are different arrays for Accent Core vs Plus R.
     *  Each array is indexed with a `ggxxacpr::EntityId` value as returned by `ggxxacpr::Player::id`.
     * 
     *  \param gameVer Which version of array to get
     */
    int16_t* GetPushboxAirborneOffsetArray(ggxxacpr::GameVersion gameVer) {
        return _ref->GetPushboxAirborneOffsetArray(static_cast<int32_t>(gameVer));
    }
    /**
     *  \brief Returns a pointer to the throw range array specified by the `type` parameter.
     * 
     *  These arrays are all indexed with an `ggxxacpr::EntityId` value as returned by `ggxxacpr::Player::id`.
     * 
     *  \param type The type of array to get
     */
    int16_t* GetThrowRangeArray(ThrowRangeArrayType type) {
        return _ref->GetThrowRangeArray(static_cast<int32_t>(type));
    }
    /**
     *  \brief Returns a pointer to the command throw range array.
     * 
     *  This array is indexed with a command grab id.
     */
    uint16_t* GetCommandGrabRangeArray() {
        return _ref->GetCommandGrabRangeArray();
    }
};

class GameDataApi : public ApiWrapper<BaseMod_GameDataApi> {
public:
    GameDataApi() :
        ApiWrapper(),
        CharacterData() { }
    GameDataApi(const BaseMod_GameDataApi* ref) :
        ApiWrapper(ref),
        CharacterData(ref->CharacterData) { }

    /**
     *  \brief API for accessing static character data.
     */
    CharDataApi CharacterData;

    /**
     *  \brief Gets player data.
     * 
     *  Player entities are not initialized until the first time the battle screen loads.
     *  Invalid `ggxxacpr::Player` views may be returned by this function. Check their
     *  validity with `ggxxacpr::Player::IsValid()`.
     * 
     *  \param playerIndex 0 for P1, 1 for P2
     *  \return A view on the player data.
     */
    ggxxacpr::Player GetPlayer(int playerIndex) {
        return _ref->GetPlayer(playerIndex);
    }
    /**
     *  \brief Gets a pointer to the player input struct array. See `GGXXACPR_PlayerInput`.
     * 
     *  The array returned is size 2, first entry is player 1's input and second is player 2's.
     * 
     *  \return A pointer to the player input struct array.
     */
    ggxxacpr::PlayerInput* GetPlayerInputStructArr() { return _ref->GetPlayerInputStructArr(); }
    /**
     *  \brief Gets camera data.
     */
    ggxxacpr::Camera GetCamera() {
        return _ref->GetCamera();
    }
    /**
     *  \brief returns a non-zero value if the game is on the battle screen.
     */
    bool IsInGame() { return _ref->IsInGame() > 0; }
    /**
     *  \brief Returns the current job mode, see enum `GGXXACPR_JobMode`. This variable
     *      determines what scene the game is set to such as "TitleScreen", "Battle", "MissionMenu".
     */
    ggxxacpr::JobMode GetJobMode() { return static_cast<ggxxacpr::JobMode>(_ref->GetJobMode()); }
    /**
     *  \brief Returns the current game mode feature flags.
     */
    ggxxacpr::GameModeFeatureFlags GetGameModeFeatureFlags() { return static_cast<ggxxacpr::GameModeFeatureFlags>(_ref->GetGameModeFeatureFlags()); }
    /**
     *  \brief Returns the item selected from the main menu.
     * 
     *  This value defaults to MAIN_MENU_ITEM_ARCADE and is
     *  set when selecting an option on the main menu.
     */
    ggxxacpr::MenuItem GetMainMenuSelection() { return static_cast<ggxxacpr::MenuItem>(_ref->GetMainMenuSelection()); }
    /**
     *  \brief Returns the D3D9 device pointer.
     * 
     *  Include `d3d9.h` and cast to IDirect3DDevice9 to use. This is
     *  a borrowed pointer. Do not call `Release()`.
     */
    void* GetD3D9Device() { return _ref->GetD3D9Device(); }
    /**
     *  \brief Gets the current game version.
     * 
     *  AC or AC+R as selected from the "Main Menu > help & options > Game Settings > "Version" setting.
     */
    ggxxacpr::GameVersion GetGameVersion() { return static_cast<ggxxacpr::GameVersion>(_ref->GetGameVersion()); }
    /**
     *  \brief Returns the current view width
     */
    uint32_t GetViewWidth() { return _ref->GetViewWidth(); }
    /**
     *  \brief Returns the current view height
     */
    uint32_t GetViewHeight() { return _ref->GetViewHeight(); }
    /**
     *  \brief Returns a pointer to the root entity.
     * 
     *  This entity is the root node of the entity linked list. Iterate through it
     *  using `ggxxacpr::Entity::next` and `ggxxacpr::Entity::prev`.
     */
    ggxxacpr::Entity GetRootEntity() { return ggxxacpr::Entity(_ref->GetRootEntity()); }
    /**
     *  \brief Gets raw throw detection info
     */
    uint32_t GetGlobalThrowFlags() { return _ref->GetGlobalThrowFlags(); }
    /**
     *  \brief Returns the pause state.
     * 
     *  The game is paused if this value is non zero. In training mode, this variable transitions from 0 to 1 to 2 when pausing.
     */
    int32_t GetPauseState() { return *_ref->GetPauseState(); }
    /**
     *  \brief Returns the pause display state.
     * 
     *  The pause menu should be drawn if this function returns a non-zero value.
     */
    int32_t GetPauseDisplayState() { return *_ref->GetPauseDisplayState(); }
    /**
     *  \brief Gets a pointer to the game's locale state. see `GGXXACPR_LocaleState`.
     */
    ggxxacpr::LocaleState* GetLocaleState() { return _ref->GetLocaleState(); }
};

template<typename T>
using PeekMessageHook = void (BASEMOD_CALL *)(T* userData, const HookContext* ctx, const PeekMessageInfo* info);
template<typename T>
using GameUpdateHook = void(BASEMOD_CALL *)(T* userData, const HookContext* ctx, const GameUpdateInfo* info);
template<typename T>
using DrawHook = void(BASEMOD_CALL *)(T* userData, const HookContext* ctx, const DrawInfo* info);
template<typename T>
using SaveGameHook = void(BASEMOD_CALL *)(T* userData, const HookContext* ctx, const SaveGameInfo* info);

class HookApi : public ApiWrapper<BaseMod_HookApi> {
public:
    /**
     *  \brief Removes a hook from the registry.
     * 
     *  \param id The `BaseMod::HookId` of the hook to be removed.
     *  \return zero if no error occurred, otherwise returns the error code.
     */
    uint32_t RemoveHook(HookId id) {
        return _ref->RemoveHook(id);
    }
    /**
     *  \brief Registers a hook to the PeekMessage hook.
     * 
     *  Use this to read windows message such as low level keyboard inputs.
     *  This hook is a call replacement and not a direct `PeekMessage` hook
     *  so callbacks are only called for messages retrieved by the game window.
     *  The parameters used to call `PeekMessage` are provided in the
     *  `BaseMod_PeekMessageInfo` parameter of `BaseMod_PeekMessageHook`.
     * 
     *  \param hookFn The callback function, see type `BaseMod_PeekMessageHook`.
     *  \param userData A generic pointer to state data that will be forwarded to `hookFn`.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    HookId AfterPeekMessage(BaseMod_PeekMessageHook hookFn, void* userData) {
        return _ref->AfterPeekMessage(hookFn, userData);
    }
    /**
     *  \brief Registers a hook to the PeekMessage hook.
     * 
     *  Use this to read windows message such as low level keyboard inputs.
     *  This hook is a call replacement and not a direct `PeekMessage` hook
     *  so callbacks are only called for messages retrieved by the game window.
     *  The parameters used to call `PeekMessage` are provided in the
     *  `BaseMod_PeekMessageInfo` parameter of `BaseMod::PeekMessageHook<T>`.
     *      Template version for type safety on the userData pointer.
     * 
     *  \param hookFn The callback function, see type `PeekMessageHook<T>`.
     *  \param userData A generic pointer to state data that will be forwarded to `hookFn`.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    template<typename T>
    HookId AfterPeekMessage(PeekMessageHook<T> hookFn, T* userData) {
        return _ref->AfterPeekMessage(
            reinterpret_cast<BaseMod_PeekMessageHook>(hookFn),
            userData);
    }
    /**
     *  \brief Registers a hook to run before the game state updates.
     * 
     *  Use this to apply changes to the game state right before it runs an update.
     * 
     *  \param hookFn The callback function, see type `BaseMod_GameUpdateHook`.
     *  \param userData A generic pointer to state data that will be forwarded to `hookFn`.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    HookId BeforeGameUpdate(BaseMod_GameUpdateHook hookFn, void* userData) {
        return _ref->BeforeGameUpdate(hookFn, userData);
    }
    /**
     *  \brief Registers a hook to run before the game state updates.
     * 
     *  Use this to apply changes to the game state right before it runs an update.
     *      Template version for type safety on the userData pointer.
     * 
     *  \param hookFn The callback function, see type `GameUpdateHook<T>`.
     *  \param userData A generic pointer to state data that will be forwarded to `hookFn`.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    template<typename T>
    HookId BeforeGameUpdate(GameUpdateHook<T> hookFn, T* userData) {
        return _ref->BeforeGameUpdate(
            reinterpret_cast<BaseMod_GameUpdateHook>(hookFn),
            userData);
    }
    /**
     *  \brief Registers a hook to run after the game state updates.
     * 
     *  Use this to read game state as soon as it updates or to overwrite
     *      the game state right after the game updates it.
     * 
     *  \param hookFn The callback function, see type `BaseMod_GameUpdateHook`.
     *  \param userData A generic pointer to state data that will be forwarded to `hookFn`.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    HookId AfterGameUpdate(BaseMod_GameUpdateHook hookFn, void* userData) {
        return _ref->AfterGameUpdate(hookFn, userData);
    }
    /**
     *  \brief Registers a hook to run after the game state updates.
     * 
     *  Use this to read game state as soon as it updates or to overwrite
     *      the game state right after the game updates it.
     *      Template version for type safety on the userData pointer.
     * 
     *  \param hookFn The callback function, see type `GameUpdateHook<T>`.
     *  \param userData A generic pointer to state data that will be forwarded to `hookFn`.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    template<typename T>
    HookId AfterGameUpdate(GameUpdateHook<T> hookFn, T* userData) {
        return _ref->AfterGameUpdate(
            reinterpret_cast<BaseMod_GameUpdateHook>(hookFn),
            userData);
    }
    /**
     *  \brief Registers a hook to run before the call to `IDirect3DDevice9::EndScene`.
     * 
     *  Use this to add additional graphics logic to the game's main scene.
     * 
     *  \param hookFn The callback function, see type `BaseMod_DrawHook`.
     *  \param userData A generic pointer to state data that will be forwarded to `hookFn`.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    HookId BeforeEndScene(BaseMod_DrawHook hookFn, void* userData) {
        return _ref->BeforeEndScene(hookFn, userData);
    }
    /**
     *  \brief Registers a hook to run before the call to `IDirect3DDevice9::EndScene`.
     * 
     *  Use this to add additional graphics logic to the game's main scene.
     *      Template version for type safety on the userData pointer.
     * 
     *  \param hookFn The callback function, see type `DrawHook<T>`.
     *  \param userData A generic pointer to state data that will be forwarded to `hookFn`.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    template<typename T>
    HookId BeforeEndScene(DrawHook<T> hookFn, T* userData) {
        return _ref->BeforeEndScene(
            reinterpret_cast<BaseMod_DrawHook>(hookFn),
            userData);
    }
    /**
     *  \brief Registers a hook to run before the call to `IDirect3DDevice9::Present`.
     * 
     *  Use this to add a new scene to the current frame. Hooks registered must start and
     *      end their own scene with `IDirect3DDevice9::BeginScene` and `IDirect3DDevice9::EndScene`.
     * 
     *  \param hookFn The callback function, see type `BaseMod_DrawHook`.
     *  \param userData A generic pointer to state data that will be forwarded to `hookFn`.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    HookId BeforePresent(BaseMod_DrawHook hookFn, void* userData) {
        return _ref->BeforePresent(hookFn, userData);
    }
    /**
     *  \brief Registers a hook to run before the call to `IDirect3DDevice9::Present`.
     * 
     *  Use this to add a new scene to the current frame. Hooks registered must start and
     *      end their own scene with `IDirect3DDevice9::BeginScene` and `IDirect3DDevice9::EndScene`.
     *      Template version for type safety on the userData pointer.
     * 
     *  \param hookFn The callback function, see type `DrawHook<T>`.
     *  \param userData A generic pointer to state data that will be forwarded to `hookFn`.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    template<typename T>
    HookId BeforePresent(DrawHook<T> hookFn, T* userData) {
        return _ref->BeforePresent(
            reinterpret_cast<BaseMod_DrawHook>(hookFn),
            userData);
    }
    /**
     *  \brief Registers a hook to run when the game saves such as on "Saving..." auto saving screens
     *      Or when selecting the menu option: "Main Menu > HELP & OPTIONS > SAVE / LOAD > SAVE".
     * 
     *  Use this hook to trigger saving mod data.
     * 
     *  \param hookFn The callback function, see type `BaseMod_SaveGameHook`.
     *  \param userData A generic pointer to state data that will be forwarded to `hookFn`.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    HookId AfterSaveGame(BaseMod_SaveGameHook hookFn, void* userData) {
        return _ref->AfterSaveGame(hookFn, userData);
    }
    /**
     *  \brief Registers a hook to run when the game saves such as on "Saving..." auto saving screens
     *      Or when selecting the menu option: "Main Menu > HELP & OPTIONS > SAVE / LOAD > SAVE".
     * 
     *  Use this hook to trigger saving mod data.
     * 
     *  \param hookFn The callback function, see type `SaveGameHook<T>`.
     *  \param userData A generic pointer to state data that will be forwarded to `hookFn`.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    template<typename T>
    HookId AfterSaveGame(SaveGameHook<T> hookFn, T* userData) {
        return _ref->AfterSaveGame(
            reinterpret_cast<BaseMod_SaveGameHook>(hookFn),
            userData
        );
    }
};

using MenuAction = BM_MenuAction;
using ValueChangeCallback = BM_ValueChangeCallback;
using CustomMenuHandler = BM_CustomMenuHandler;
/// \copydoc BaseMod_ModMenuEntry
using ModMenuEntry = BaseMod_ModMenuEntry;
using MenuDimensions = BaseMod_MenuDimensions;

class ModMenuHelperFunctionsApi : public ApiWrapper<BaseMod_ModMenu_HelperFunctionsApi> {
public:
    /**
     *  \brief Handles updating the current menu selection.
     * 
     *  Updates selection based on player input and implements hold-repeat functionality identically to native menus.
     * 
     *  \param currentSelection The selection value. 0 is the top menu item.
     *  \param totalEntries Number of entries in the current menu (i.e. maximum value + 1).
     *      Needed for selection wrapping.
     */
    int32_t SelectionHandler(int32_t currentSelection, uint32_t totalEntries) {
        return _ref->SelectionHandler(currentSelection, totalEntries);
    }
    /**
     *  \brief Handles input repeating for held direction inputs.
     * 
     *  Uses this for left/right inputs in the menu or custom selection handling.
     * 
     *  \param input The direction to check
     *  \return true if the input should be processed for a repeat
     */
    bool HoldDirectionInputHandler(ggxxacpr::RawControllerInput input) {
        return _ref->HoldDirectionInputHandler(static_cast<uint32_t>(input));
    }
    /**
     *  \brief Draws the enum setting UI.
     * 
     *  \param label Label of the current value. This label is drawn via the `RenderCockpitFontText` internal function.
     *      String format and character restrictions apply. see `BaseMod::NativeFunctionsApi::RenderCockpitFontText`
     *      for details.
     *  \param xOffset Offset from the default x position. Internal resolution screen-space coordinates.
     *  \param y Internal resolution screen-space coordinate (640x480). Top edge is 0, bottom is 480.
     *  \param isSelected A 4-byte boolean that determines transparency.
     *      Opaque if param is non-zero, semi-transparent if zero.
     */
    void DrawEnumSettingUI(const char* label, int32_t xOffset, int32_t y, int32_t isSelected) {
        _ref->DrawEnumSettingUI(label, xOffset, y, isSelected);
    }
    /**
     *  \brief Draws the gauge setting UI.
     * 
     *  The native function invoked here has a hard-coded x position.
     * 
     *  \param currentValue Value to display on the gauge.
     *  \param y Internal resolution screen-space coordinate (640x480). Top edge is 0, bottom is 480.
     *  \param isSelected A 4-byte boolean that determines transparency.
     *      Opaque if param is non-zero, semi-transparent if zero.
     *  \param maxValue Maximum value the gauge should display.
     */
    void DrawGaugeSettingUI(int32_t currentValue, int32_t y, int32_t isSelected, int32_t maxValue) {
        _ref->DrawGaugeSettingUI(currentValue, y, isSelected, maxValue);
    }
    /**
     *  \brief Draws one or both of the menu scroll arrows.
     * 
     *  This is the same sprite as the scroll arrows in the training menu but with custom positions.
     * 
     *  \param flags controls which arrows are drawn.
     */
    void DrawScrollArrow(DrawScrollArrowFlags flags) {
        _ref->DrawScrollArrow(static_cast<int32_t>(flags));
    }
    /**
     *  \brief Yields control back to the main fiber. Use this to pause
     *      `BaseMod::CustomMenuHandler` execution until the next frame. Useful for sub-menus.
     * 
     *  See doc on [Fibers](https://YouKnow232.github.io/GearLoader/doxygen/md_docs_2_game_architecture).
     */
    void SwitchToMainFiber() {
        _ref->SwitchToMainFiber();
    }
};

class ModMenuApi : public ApiWrapper<BaseMod_ModMenuApi> {
public:
    ModMenuApi() :
        ApiWrapper(),
        HelperFunctions() { }
    ModMenuApi(const BaseMod_ModMenuApi* ref) :
        ApiWrapper(ref),
        HelperFunctions(ref->HelperFunctions) {}

    ModMenuHelperFunctionsApi HelperFunctions;

    /**
     *  \brief Get's a pointer to a struct defining the bounds of the drawable menu area.
     * 
     *  Use these dimensions to future proof `BM_CustomMenuHandler` implementations.
     */
    MenuDimensions GetDrawableAreaDimensions() {
        auto dim = _ref->GetDrawableAreaDimensions();
        return {dim->left, dim->top, dim->right, dim->bottom};
    }
    /**
     *  \brief Registers a menu tab definition with the mod menu.
     * 
     *  \param title Tab name. Character limitations are similar to `BaseMod_NativeFunctionsApi::RenderCockpitFontText`
     *  \param entries An array of BaseMod_ModMenuEntry structures comprising the menu definition.
     *      See `BaseMod_ModMenuEntry`. Callers must maintain the lifetime of values in the declaration.
     *  \return 0 if no error, else an error code.
     */
    uint32_t RegisterMenuTab(const char* title, const ModMenuEntry* entries, uint32_t numEntries) {
        return _ref->RegisterMenuTab(title, entries, numEntries);
    }
    /**
     *  \brief Registers a custom menu handler function.
     * 
     *  \param title Tab name. Character limitations are similar to `BaseMod_NativeFunctionsApi::RenderCockpitFontText`
     *  \param handler Custom menu handler function invoked by the Mod Menu Manager. This callback is invoked
     *      after the Mod Menu handles fiber switching, tab switching, and exiting the menu, so there is no
     *      need to implement that functionality in this callback. It's recommend to use the dimensions from
     *      `GetDrawableAreaDimensions` to future proof your implementation. Refer to the source code at
     *      `source/baseMod/modMenu/modMenu.cpp::ModMenu()` for an example of implementing your own handler.
     */
    uint32_t RegisterCustomMenuTab(const char* title, CustomMenuHandler handler) {
        return _ref->RegisterCustomMenuTab(title, handler);
    }
};

class Api : public ApiWrapper<BaseMod_Api> {
public:
    Api()
        : ApiWrapper()
        , NativeFunctions()
        , GameData()
        , Hooks()
        , ModMenu() { }
    Api(const BaseMod_Api* ref)
        : ApiWrapper(ref)
        , NativeFunctions(ref->NativeFunctions)
        , GameData(ref->GameData)
        , Hooks(ref->Hooks)
        , ModMenu(ref->ModMenu) { }

    /// \brief API for invoking native game functions
    NativeFunctionsApi NativeFunctions;
    /// \brief Access to notable game data
    GameDataApi GameData;
    /// \brief Function hooking manager
    HookApi Hooks;
    /// \brief Add options to mod menu
    ModMenuApi ModMenu;
};

}

#endif