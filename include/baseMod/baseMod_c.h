#ifndef BASEMOD_H
#define BASEMOD_H

#define BASEMOD_NAME "baseMod"
#define BASEMOD_API_VERSION "1.1.0"
#define BASEMOD_API_VERSION_NUM 0x010100
#define BASEMOD_CALL __stdcall

#ifdef __cplusplus
    #include <cstdint>
#else
    #include <stdint.h>
#endif

#include "gearLoader/ggxxacpr_c.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum BM_DrawArrowSpriteDirection {
    BM_DASD_LEFT       = 0x01,
    BM_DASD_RIGHT      = 0x02,
    BM_DASD_UP         = 0x04,
    BM_DASD_DOWN       = 0x08,
    BM_DASD_UP_LEFT    = 0x10,
    BM_DASD_UP_RIGHT   = 0x20,
    BM_DASD_DOWN_LEFT  = 0x40,
    BM_DASD_DOWN_RIGHT = 0x80,
} BM_DrawArrowSpriteDirection;
typedef enum BM_DrawScrollArrowFlags {
    BM_DSAF_NONE = 0,
    BM_DSAF_UP   = 1,
    BM_DSAF_DOWN = 2,
    BM_DSAF_BOTH = 3,
} BM_DrawScrollArrowFlags;

struct BaseMod_NativeFunctionsApi {
    /// \brief The size of the struct in bytes
    uint32_t size;
    /// \brief The API version
    uint32_t version;

    /**
     *  \brief Draws cockpit font text to the screen.
     * 
     *  The font drawn depends on the "Main Menu > Help & Options > Display & Audio Settings > Cockpit Font" setting.
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
    void BASEMOD_CALL (*RenderCockpitFontText)(const char* text, int32_t x, int32_t y, float z, uint8_t alpha, float size);
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
     *  \param animCounter Optional pointer to an animation counter. If defined, the text
     *      will perform a 'typewriter' animation drawing an additional letter at a time
     *      until fully rendered. One letter will be rendered per 128 units on this counter.
     *      When the animation is complete, this value will be set to -1 and will no longer increment.
     *  \param animSpeed The amount to increment the `animCounter` if it is defined. Set to 128 for 1 letter per frame.
     *  \param ignoreSpriteMask A 4-byte boolean value. If a non-zero value is passed, ignores
     *      color and alpha params and draws as fully opaque white text.
     *  \param color The color of the text string (0xRRGGBB)
     */
    void BASEMOD_CALL (*RenderMenuText)(const char* text, int32_t x, int32_t y, float z,
        float alpha, int32_t* animCounter, int32_t animSpeed, int32_t ignoreSpriteMask, uint32_t color);
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
     *  \param ignoreSpriteMask A 4-byte boolean value. If a non-zero value is passed, ignores
     *      color and alpha params and draws as fully opaque white text.
     */
    void BASEMOD_CALL (*RenderMenuTextCenterAligned)(const char* text, int32_t x, int32_t y,
        float z, float alpha, uint32_t color, int32_t ignoreSpriteMask);
    /**
     *  \brief A higher level text rendering function that triggers a text popup animation in-game (e.g. COUNTER HIT / RECOVERY).
     * 
     *  \param playerIndex Which side of the screen to display the pop up.
     *  \param text The text to be displayed. See `RenderCockpitFontText` for format and available characters.
     */
    void BASEMOD_CALL (*RenderPopUpText)(int32_t playerIndex, const char* text);

    /**
     *  \brief Plays a sound effect.
     * 
     *  An internal array is indexed with the `id` parameter without validity checks.
     *  Invalid values can cause invalid memory access.
     * 
     *  \param id The given id maps to the "COMMON SE" sound effect in the Sound menu.
     *      See `github.com/youknow232/gearloader/tree/main/docs/SoundEffectIdMap.txt` for the id mappings.
     *  \return This function will return 1 if the `id` parameter is 0, otherwise it returns 0.
     */
    uint32_t BASEMOD_CALL (*PlayCommonSoundEffect)(uint32_t id);

    /**
     *  \brief Draws a sprite to the screen.
     * 
     *  See `GGXXACPR_DrawSpriteParams`
     * 
     *  \param params Combined parameter struct
     *  \param ignoreSpriteMask A 4-byte boolean value. If a non-zero value is passed, ignores
     *      color and alpha params.
     */
    void BASEMOD_CALL (*DrawSprite)(GGXXACPR_DrawSpriteParams* params, int32_t ignoreSpriteMask);

    /**
     *  \brief `DrawSprite` wrapper function that draws an arrow sprite.
     * 
     *  \param directionFlag see enum `BM_DrawArrowSpriteDirection`
     *  \param x Internal resolution screen-space coordinate (640x480). Left edge is 0, right is 640.
     *  \param y Internal resolution screen-space coordinate (640x480). Top edge is 0, bottom is 480.
     *  \param z The draw order/depth buffer value. Lower values draw later / appear in front of other text and sprites.
     *  \param alpha Transparency value from 1 to 255, 1 being fully opaque.
     */
    void BASEMOD_CALL (*DrawArrowSprite)(uint32_t directionFlag, int32_t x, int32_t y, int32_t z, uint32_t alpha);

    /**
     *  \brief Draws a triangle strip primitive.
     * 
     *  The coordinates of the vertices are given in internal resolution screen space coordinates.
     *  The top left pixel is (0, 0) and the bottom right pixel is (640, 480).
     * 
     *  \param vertices The vertices making up the triangle strip.
     *  \param numVertices The number of vertices.
     */
    void BASEMOD_CALL (*DrawTriStrip)(GGXXACPR_ColorVertex* vertices, uint32_t numVertices);
    
    /**
     *  \brief Draws a rectangle to the screen.
     * 
     *  A simple helper function that calls constructs four vertices from the given
     *  parameters and passes them to `DrawTriStrip`. The edges of the quad are
     *  given in internal resolution screen space coordinates. The top left pixel
     *  is (0, 0) and the bottom right pixel is (640, 480).
     * 
     *  This function should only be invoked in the context of the game's main scene, such
     *      as during a hook registered with `BaseMod_Api::BeforeEndScene`.
     * 
     *  \param left edge position
     *  \param top edge position
     *  \param right edge position
     *  \param bottom edge position
     *  \param z The Z coordinate for each vertex
     *  \param color ARGB color value (i.e. `D3DCOLOR` from `D3D9Types.h`)
     */
    void BASEMOD_CALL (*DrawQuad)(int32_t left, int32_t top, int32_t right, int32_t bottom, int32_t z, uint32_t color);

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
     *  \param spriteId The initial slot sprite data will be loaded into.
     *  \param textureDataArray The raw sprite data.
     *  \param count The number of sprites to register. If 0, sprites will be registered until
     *      a null pointer is found in the given array.
     *  \return The number of sprites registered
     */
    uint32_t BASEMOD_CALL (*RegisterSprites)(uint32_t spriteId, void* textureDataArray, uint32_t count);

    /**
     *  \brief If the given player currently has an active command grab,
     *      returns its command grab id, else return 0.
     */
    uint32_t BASEMOD_CALL (*GetActiveCommandGrabId)(GGXXACPR_Entity* player);
};


typedef enum BM_PushboxDimensionArrayType {
    BM_PD_STANDING_WIDTH,
    BM_PD_STANDING_HEIGHT_AC,
    BM_PD_STANDING_HEIGHT_PR,
    BM_PD_CROUCHING_WIDTH,
    BM_PD_CROUCHING_HEIGHT,
    BM_PD_AIRBORNE_WIDTH,
    BM_PD_AIRBORNE_HEIGHT,
} BM_PushboxDimensionArrayType;
typedef enum BM_ThrowRangeArrayType {
    BM_TR_GROUND_AC,
    BM_TR_GROUND_PR,
    BM_TR_AIR_HORIZONTAL_AC,
    BM_TR_AIR_HORIZONTAL_PR,
    BM_TR_AIR_UPPER,    //! This array is not split between Accent Core and Plus R
    BM_TR_AIR_LOWER,    //! This array is not split between Accent Core and Plus R
} BM_ThrowRangeArrayType;

struct BaseMod_CharDataApi {
    /// \brief The size of the struct in bytes
    uint32_t size;
    /// \brief The API version
    uint32_t version;

    /**
     *  \brief Returns a pointer to the push box array specified by the `type` parameter.
     * 
     *  These arrays are all indexed with an `GGXXACPR_EntityId` value as returned by `GGXXACPR_Entity::id`.
     * 
     * \param type see enum `BM_PushboxDimensionArrayType`
     */
    uint16_t* BASEMOD_CALL (*GetPushboxDimensionArray)(int32_t type);
    /**
     *  \brief Returns a pointer to the game's airborne pushbox offset array.
     * 
     *  These values are subtracted from the player's y position when calculating airborne pushbox collisions.
     *  There are different arrays for Accent Core vs Plus R.
     *  Each array is indexed with a `GGXXACPR_EntityId` value as returned by `GGXXACPR_Entity::id`.
     * 
     *  \param gameVer see enum `GGXXACPR_GameVersion`
     */
    int16_t* BASEMOD_CALL (*GetPushboxAirborneOffsetArray)(int32_t gameVer);
    /**
     *  \brief Returns a pointer to the throw range array specified by the `type` parameter.
     * 
     *  These arrays are all indexed with an `GGXXACPR_EntityId` value as returned by `GGXXACPR_Entity::id`.
     * 
     *  \param type see enum `BM_ThrowRangeArrayType`
     */
    int16_t* BASEMOD_CALL (*GetThrowRangeArray)(int32_t type);
    /**
     *  \brief Returns a pointer to the command throw range array.
     * 
     *  This array is indexed with a command grab id.
     */
    uint16_t* BASEMOD_CALL (*GetCommandGrabRangeArray)();
};

struct BaseMod_GameDataApi {
    /// \brief The size of the struct in bytes
    uint32_t size;
    /// \brief The API version
    uint32_t version;
    
    /**
     *  \brief API for accessing static character data.
     */
    const struct BaseMod_CharDataApi* CharacterData;

    /**
     *  \brief Gets player data.
     * 
     *  Player entities are not loaded until the first time the battle screen loads.
     *  Null checks must be performed on this function's output.
     *  Player entities are stored sequentially, so you can treat the player 1 pointer as an
     *  array containing both player structs (ex. `GGXXACPR_Entity p2 = GetPlayer(0)[1];`).
     * 
     *  \param playerIndex 0 for P1, 1 for P2
     *  \return A pointer to the specified player entity struct or the null pointer if the player structs are not yet initialized
     */
    GGXXACPR_Entity* BASEMOD_CALL (*GetPlayer)(int playerIndex);
    /**
     *  \brief Gets a pointer to the player input struct array. See `GGXXACPR_PlayerInput`.
     * 
     *  The array returned is size 2, first entry is player 1's input and second is player 2's.
     * 
     *  \return A pointer to the player input struct array.
     */
    GGXXACPR_PlayerInput* BASEMOD_CALL (*GetPlayerInputStructArr)();
    /**
     *  \brief Gets camera data.
     */
    GGXXACPR_Camera* BASEMOD_CALL (*GetCamera)();
    /**
     *  \brief returns a non-zero value if the game is on the battle screen.
     */
    int32_t BASEMOD_CALL (*IsInGame)();
    /**
     *  \brief Returns the current job mode, see enum `GGXXACPR_JobMode`. This variable
     *      determines what scene the game is set to such as "TitleScreen", "Battle", "MissionMenu".
     */
    int32_t BASEMOD_CALL (*GetJobMode)();
    /**
     *  \brief See enum `GGXXACPR_GameModeFeatureFlags`. Returns the current game mode feature flags.
     */
    uint32_t BASEMOD_CALL (*GetGameModeFeatureFlags)();
    /**
     *  \brief Enum `GGXXACPR_MainMenuItem`. Returns the game mode selected from the main menu.
     * 
     *  This value defaults to MAIN_MENU_ITEM_ARCADE and is
     *  set when selecting an option on the main menu.
     */
    uint32_t BASEMOD_CALL (*GetMainMenuSelection)();
    /**
     *  \brief Returns the D3D9 device pointer.
     * 
     *  Include `d3d9.h` and cast to IDirect3DDevice9 to use. This is
     *  a borrowed pointer. Do not call `Release()`.
     */
    void* BASEMOD_CALL (*GetD3D9Device)();
    /**
     *  \brief Gets the current game version. See enum `GGXXACPR_GameVersion`.
     * 
     *  AC or AC+R as selected from the "Main Menu > help & options > Game Settings > "Version" setting.
     */
    uint32_t BASEMOD_CALL (*GetGameVersion)();
    /**
     *  \brief Returns the current view width
     */
    uint32_t BASEMOD_CALL (*GetViewWidth)();
    /**
     *  \brief Returns the current view height
     */
    uint32_t BASEMOD_CALL (*GetViewHeight)();
    /**
     *  \brief Returns a pointer to the root entity.
     * 
     *  This entity is the root node of the entity linked list. Iterate through it
     *  using `GGXXACPR_Entity::nextPtr` and `GGXXACPR_Entity::prevPtr`.
     */
    GGXXACPR_Entity* BASEMOD_CALL (*GetRootEntity)();
    /**
     *  \brief Gets raw throw detection info
     */
    uint32_t BASEMOD_CALL (*GetGlobalThrowFlags)();
    /**
     *  \brief Gets a pointer to the pause state global variable.
     * 
     *  The game is paused if this value is non zero. In training mode, this variable transitions from 0 to 1 to 2 when pausing.
     */
    int32_t* BASEMOD_CALL (*GetPauseState)();
    /**
     *  \brief Gets a pointer to the pause display state global variable.
     * 
     *  The pause menu should be drawn if the value at this address is non-zero.
     */
    int32_t* BASEMOD_CALL (*GetPauseDisplayState)();
    /**
     *  \brief Gets a pointer to the game's locale state. see `GGXXACPR_LocaleState`.
     */
    GGXXACPR_LocaleState* BASEMOD_CALL (*GetLocaleState)();
};

typedef uint32_t BaseMod_HookId;
typedef struct BaseMod_HookContext BaseMod_HookContext;

typedef struct BaseMod_PeekMessageArgs {
    void* lpMsg;    //! MSG from `winuser.h`
    void* hWnd;     //! HWND from `WinDef.h`
    uint32_t wMsgFilterMin;
    uint32_t wMsgFilterMax;
    uint32_t wRemoveMsg;
} BaseMod_PeekMessageArgs;
typedef struct BaseMod_PeekMessageInfo {
    BaseMod_PeekMessageArgs args;
    int success;
} BaseMod_PeekMessageInfo;
typedef void BASEMOD_CALL (*BaseMod_PeekMessageHook)(void* userData, const BaseMod_HookContext* ctx, const BaseMod_PeekMessageInfo* info);

//! Reserved
typedef struct BaseMod_GameUpdateInfo BaseMod_GameUpdateInfo;
//! Parameters reserved
typedef void BASEMOD_CALL (*BaseMod_GameUpdateHook)(void* userData, const BaseMod_HookContext* ctx, const BaseMod_GameUpdateInfo* info);

typedef struct BaseMod_DrawInfo {
    //! include `d3d9.h` and cast to IDirect3DDevice9*.
    void* device;
} BaseMod_DrawInfo;
typedef void BASEMOD_CALL (*BaseMod_DrawHook)(void* userData, const BaseMod_HookContext* ctx, const BaseMod_DrawInfo* info);
//! Reserved
typedef struct BaseMod_SaveGameInfo {
    //! Pointer to an undocumented save file structure
    void* saveFileStruct;
} BaseMod_SaveGameInfo;
typedef void BASEMOD_CALL (*BaseMod_SaveGameHook)(void* userData, const BaseMod_HookContext* ctx, const BaseMod_SaveGameInfo* info);

//! Adds callbacks to managed function hooks.
struct BaseMod_HookApi {
    /// \brief The size of the struct in bytes
    uint32_t size;
    /// \brief The API version
    uint32_t version;

    /**
     *  \brief Removes a hook from the registry.
     * 
     *  \param id The `BaseMod_HookId` of the hook to be removed.
     *  \return 0 if no error, else an error code.
     */
    uint32_t BASEMOD_CALL (*RemoveHook)(BaseMod_HookId id);
    /**
     *  \brief Registers a callback to the PeekMessage hook.
     * 
     *  Use this to read windows message such as low level keyboard inputs.
     *  This hook is a call replacement and not a direct `PeekMessage` hook
     *  so callbacks are only called for messages retrieved by the game window.
     *  The parameters used to call `PeekMessage` are provided in the
     *  `BaseMod_PeekMessageInfo` parameter of `BaseMod_PeekMessageHook`.
     * 
     *  \param hookFn The callback function, see `BaseMod_PeekMessageHook`.
     *  \param userData A generic pointer to state data that will be forwarded to `hookFn`.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    BaseMod_HookId BASEMOD_CALL (*AfterPeekMessage)(BaseMod_PeekMessageHook hookFn, void* userData);
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
    BaseMod_HookId BASEMOD_CALL (*BeforeGameUpdate)(BaseMod_GameUpdateHook hookFn, void* userData);
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
    BaseMod_HookId BASEMOD_CALL (*AfterGameUpdate)(BaseMod_GameUpdateHook hookFn, void* userData);
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
    BaseMod_HookId BASEMOD_CALL (*BeforeEndScene)(BaseMod_DrawHook hookFn, void* userData);
    /**
     *  \brief Registers a hook to run after the call to `IDirect3DDevice9::EndScene`
     *      but before the call to `IDirect3DDevice9::Present`.
     * 
     *  Use this to add a new scene to the current frame.
     * 
     *  \param hookFn The callback function, see type `BaseMod_DrawHook`.
     *  \param userData A generic pointer to state data that will be forwarded to `hookFn`.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    // Called outside the game's begin scene context before present is called.
    BaseMod_HookId BASEMOD_CALL (*BeforePresent)(BaseMod_DrawHook hookFn, void* userData);
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
    BaseMod_HookId BASEMOD_CALL (*AfterSaveGame)(BaseMod_SaveGameHook hookFn, void* userData);
};


typedef void (BASEMOD_CALL *BM_MenuAction)();
typedef void (BASEMOD_CALL *BM_ValueChangeCallback)(int32_t value);
/**
 *  \brief Represents a mod menu entry.
 * 
 *  Entries can display in 3 different modes depending on which values are defined in this
 *      struct.
 * 
 *  - Command:
 *    - Definition: Value is null, ValueLabels is null, and Command is defined
 *    - Behavior: Only display label, execute command when selected.
 *  - Gauge:
 *    - Definition: Value is defined and ValueLabels is null
 *    - Behavior: Set a value on a gauge (e.g. Training Menu->Life Bar)
 *  - Enum:
 *    - Definition: Value and ValueLabels are defined
 *    - Behavior: Select a value (e.g. Training Menu->Display)
 * 
 *  Number Line and Enum Value type entries may also have a command function.
 */
typedef struct BaseMod_ModMenuEntry {
    /**
     *  \brief Primary label
     * 
     *  Drawn via the `RenderMenuText` native function. see
     *  `BaseMod_NativeFunctionsApi::RenderMenuText` for format and character restrictions.
     */
    const char* Label;
    /**
     *  \brief Real value behind the setting.
     * 
     *  This value will be read and written to.
     */
    int32_t* Value;
    /**
     *  \brief Minimum value. Should almost always be 0. Inclusive.
     * 
     *  Gauge setting UI assumes minimum is 0. Other values may display strangely.
     */
    int32_t MinValue;
    /**
     *  \brief Maximum value for Number Line and maximum index for value labels (i.e. `ValueLabel` size - 1).
     * 
     *  Inclusive. Getting this value wrong can cause crashes!
     */
    int32_t MaxValue;
    /**
     *  \brief A list of labels to assign to each value.
     * 
     *  Value labels are drawn via the `RenderCockpitFontText` internal function. String format and character
     *      restrictions apply. see `BaseMod_NativeFunctionsApi::RenderCockpitFontText` for details.
     */
    const char** ValueLabels;
    /// \brief Optional command callback when entry is selected and pressed
    BM_MenuAction Command;
    /// \brief Optional value changed callback. Can be used to update other values based on the entry `Value`.
    BM_ValueChangeCallback ValueChanged;
} BaseMod_ModMenuEntry;

/**
 *  \brief Rectangle dimensions bounding the drawable menu area given in internal resolution
 *      screen space coordinates. Top left pixel is (0, 0) and bottom right is (640, 480).
 *      Use these dimensions to future proof `BM_CustomMenuHandler` implementations in case
 *      the mod menu display changes.
 */
typedef struct BaseMod_MenuDimensions {
    uint16_t left;
    uint16_t top;
    uint16_t right;
    uint16_t bottom;
} BaseMod_MenuDimensions;

typedef struct BaseMod_ModMenu_HelperFunctionsApi {
    /// \brief The size of the struct in bytes
    uint32_t size;
    /**
     *  \brief the API version.
     * 
     *  The mod loader will create this struct with the value given by the `BASEMOD_API_VERSION_NUM`
     *      macro. Compare this value with the `BASEMOD_API_VERSION_NUM` macro to detect version
     *      differences between the installed mod loader version and the targeted mod loader API version.
     *      The format is a semantic version 0x00AABBCC where 0xAA is major, 0xBB is minor, and 0xCC is patch number.
     */
    uint32_t version;

    /**
     *  \brief Handles updating the current menu selection.
     * 
     *  Updates selection based on player input and implements hold-repeat functionality identically to native menus.
     * 
     *  \param currentSelection The selection value. 0 is the top menu item.
     *  \param totalEntries Number of entries in the current menu (i.e. maximum value + 1).
     *      Needed for selection wrapping.
     */
    int32_t BASEMOD_CALL (*SelectionHandler)(int32_t currentSelection, uint32_t totalEntries);

    /**
     *  \brief Handles input repeating for held direction inputs.
     * 
     *  Uses this for left/right inputs in the menu or custom selection handling.
     * 
     *  \param input see enum `GGXXACPR_RawControllerInput`
     */
    int32_t BASEMOD_CALL (*HoldDirectionInputHandler)(uint32_t input);

    /**
     *  \brief Draws the enum setting UI.
     * 
     *  \param label Label of the current value. This label is drawn via the `RenderCockpitFontText` internal function.
     *      String format and character restrictions apply. see `BaseMod_NativeFunctionsApi::RenderCockpitFontText`
     *      for details.
     *  \param xOffset Offset from the default x position. Internal resolution screen-space coordinates.
     *  \param y Internal resolution screen-space coordinate (640x480). Top edge is 0, bottom is 480.
     *  \param isSelected A 4-byte boolean that determines transparency.
     *      Opaque if param is non-zero, semi-transparent if zero.
     */
    void BASEMOD_CALL (*DrawEnumSettingUI)(const char* label, int32_t xOffset, int32_t y, int32_t isSelected);

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
    void BASEMOD_CALL (*DrawGaugeSettingUI)(int32_t currentValue, int32_t y, int32_t isSelected, int32_t maxValue);

    /**
     *  \brief Draws one or both of the menu scroll arrows.
     * 
     *  This is the same sprite as the scroll arrows in the training menu but with custom positions.
     * 
     *  \param flags see enum `BM_DrawScrollArrowFlags`
     */
    void BASEMOD_CALL (*DrawScrollArrow)(int32_t flags);

    /**
     *  \brief Yields control back to the main fiber. Use this to pause
     *      `BM_CustomMenuHandler` execution until the next frame. Useful for sub-menus.
     * 
     *  See doc on [Plus R's Fibers](https://YouKnow232.github.io/GearLoader/doxygen/md_docs_2_game_architecture).
     */
    void BASEMOD_CALL (*SwitchToMainFiber)();

} BaseMod_ModMenu_HelperFunctionsApi;
typedef void (BASEMOD_CALL *BM_CustomMenuHandler)(GGXXACPR_PlayerInput* inputArr);

//! Add options to mod menu
struct BaseMod_ModMenuApi {
    /// \brief The size of the struct in bytes
    uint32_t size;
    /**
     *  \brief the API version.
     * 
     *  The mod loader will create this struct with the value given by the `BASEMOD_API_VERSION_NUM`
     *      macro. Compare this value with the `BASEMOD_API_VERSION_NUM` macro to detect version
     *      differences between the installed mod loader version and the targeted mod loader API version.
     *      The format is a semantic version 0x00AABBCC where 0xAA is major, 0xBB is minor, and 0xCC is patch number.
     */
    uint32_t version;
    
    /**
     *  \brief A collection of helper functions for implementing your own custom menu handler.
     */
    const BaseMod_ModMenu_HelperFunctionsApi* HelperFunctions;
    /**
     *  \brief Gets a pointer to a struct defining the bounds of the drawable menu area.
     * 
     *  Use these dimensions to future proof `BM_CustomMenuHandler` implementations.
     */
    const BaseMod_MenuDimensions* BASEMOD_CALL (*GetDrawableAreaDimensions)();
    /**
     *  \brief Registers a menu tab definition with the mod menu.
     * 
     *  \param title Tab name. Character limitations are similar to `BaseMod_NativeFunctionsApi::RenderCockpitFontText`
     *  \param entries An array of BaseMod_ModMenuEntry structures comprising the menu definition.
     *      See `BaseMod_ModMenuEntry`. Callers must maintain the lifetime of values in the declaration.
     *  \return 0 if no error, else an error code.
     */
    uint32_t BASEMOD_CALL (*RegisterMenuTab)(const char* title, const BaseMod_ModMenuEntry* entries, uint32_t numEntries);
    /**
     *  \brief Registers a custom menu handler function.
     * 
     *  \param title Tab name. Character limitations are similar to `BaseMod_NativeFunctionsApi::RenderCockpitFontText`
     *  \param handler Custom menu handler function invoked by the Mod Menu Manager. This callback is invoked
     *      after the Mod Menu handles fiber switching, tab switching, and exiting the menu, so there is no
     *      need to implement that functionality in this callback. It's recommended to use the dimensions from
     *      `GetDrawableAreaDimensions` to future proof your implementation. See the `HelperFunctions` sub API
     *      and Refer to the source code at `source/baseMod/modMenu/modMenu.cpp::ModMenu()` for help
     *      implementing your own handler.
     */
    uint32_t BASEMOD_CALL (*RegisterCustomMenuTab)(const char* title, BM_CustomMenuHandler handler);
};

/**
 *  \brief Root struct for the base mod API.
 * 
 *  A pointer to an instance of this struct will be given by `GearLoaderApi::RetrieveModAPI()`.
 */
typedef struct BaseMod_Api {
    /// \brief The size of the struct in bytes
    uint32_t size;
    /**
     *  \brief the API version.
     * 
     *  The mod loader will create this struct with the value given by the `BASEMOD_API_VERSION_NUM`
     *      macro. Compare this value with the `BASEMOD_API_VERSION_NUM` macro to detect version
     *      differences between the installed mod loader version and the targeted mod loader API version.
     *      The format is a semantic version 0x00AABBCC where 0xAA is major, 0xBB is minor, and 0xCC is patch number.
     */
    uint32_t version;
    /// \brief API for invoking native game functions
    const struct BaseMod_NativeFunctionsApi* NativeFunctions;
    /// \brief Curated access to game data
    const struct BaseMod_GameDataApi* GameData;
    /// \brief Function hooking manager
    const struct BaseMod_HookApi* Hooks;
    /// \brief Add options to the mod menu
    const struct BaseMod_ModMenuApi* ModMenu;
} BaseMod_Api;

#ifdef __cplusplus
}   // extern "C"
#endif

#endif
