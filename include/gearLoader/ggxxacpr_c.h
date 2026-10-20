/*-------------------------------------------------------------------------------------------------
* This header serves as the GGXXACPR ABI which defines its native data types and structures.
*
* Mod Developers:
*  This header is included by `baseMod_c.h`. If you're forgoing the basemod
*  include this header instead to interface with the game directly.
*
* GearLoader Developers:
*  Existing definitions should only be changed to address compiler compatibility,
*  fix breaking game updates, identify unknown fields, and refine field names.
*--------------------------------------------------------------------------------------------------*/

#ifndef GGXXACPR_H
#define GGXXACPR_H

#define GGXXACPR_ABI_VERSION "1.1.0"
#define GGXXACPR_ABI_VERSION_NUM 0x010100

#include <assert.h>

#if __cplusplus
    #include <cstdint>
#else
    #include <stdint.h>
#endif

// Used for fields that have a known data type, but unknown use
#define UNKNOWN_FIELD(label) _unknown_##label
// Used for stretchs of struct data that are either unknown or unused
#define PAD(num_bytes, label) uint8_t _padding_##label[num_bytes]

static_assert(sizeof(float) == 4, "float size must be 32-bit");
static_assert(sizeof(void*) == 4, "pointer size must be 32-bit");


/* ========== #ENUMS ========== */

enum GGXXACPR_GameVersion {
    GAME_VERSION_ACCENT_CORE = 0,
    GAME_VERSION_PLUS_R
};

// Set when selecting a main menu item
enum GGXXACPR_MainMenuItem {
    MAIN_MENU_ITEM_ARCADE = 0,
    MAIN_MENU_ITEM_MOM,
    MAIN_MENU_ITEM_VERSUS,
    MAIN_MENU_ITEM_ONLINE,
    MAIN_MENU_ITEM_VS_CPU,
    MAIN_MENU_ITEM_TEAM_VERSUS,
    MAIN_MENU_ITEM_TEAM_VS_CPU,
    MAIN_MENU_ITEM_TRAINING,
    MAIN_MENU_ITEM_SURVIVAL,
    MAIN_MENU_ITEM_MISSION,
    MAIN_MENU_ITEM_STORY,
    MAIN_MENU_ITEM_GALLERY,
    MAIN_MENU_ITEM_REPLAY_THEATER,
    MAIN_MENU_ITEM_SOUND_TEST
};

// Dictates high-level program flow
enum GGXXACPR_JobMode {
    JOB_MODE_AUTO_LOAD = 0x0,
    JOB_MODE_AUTO_SAVE = 0x1,
    JOB_MODE_TITLE_SCREEN = 0x2,
    JOB_MODE_INIT_GAME_INFO = 0x3,
    JOB_MODE_GAME_INIT = 0x5,
    JOB_MODE_BATTLE = 0x6,
    JOB_MODE_DEMO = 0x7,
    JOB_MODE_TITLE_MAIN = 0x9,
    JOB_MODE_MENUS = 0xA,
    JOB_MODE_LOAD_CHAR_SELECT = 0xF,
    JOB_MODE_LOAD_STAGE_SELECT = 0x10,
    JOB_MODE_LOAD_VERSUS_SCREEN = 0x11,
    JOB_MODE_LOAD_STORY_MODE_SPLASH_SCREEN = 0x12,
    JOB_MODE_LOAD_WIN_SCREEN = 0x14,
    JOB_MODE_LOAD_STORY_ENDING = 0x16,
    JOB_MODE_OPTION_MENU = 0x19,
    JOB_MODE_DEBUG_SOUND_MENU = 0x21,
    JOB_MODE_MISSION_MENU = 0x24,
    JOB_MODE_STORY_INIT = 0x29,
    JOB_MODE_SREXEC = 0x2B,
    JOB_MODE_CREXEC = 0x2C,
    JOB_MODE_MOVIE_EXECUTE = 0x2D,
    JOB_MODE_MATC = 0x2E,
    JOB_MODE_MISSION_WIN = 0x2F,
    JOB_MODE_SURVIVAL_START = 0x30,
    JOB_MODE_GALLERY_MAIN = 0x31,
    JOB_MODE_SURVIVAL = 0x32,
    JOB_MODE_TEAM_VS = 0x33,
    JOB_MODE_FRAMERATE_TEST = 0x34,
    JOB_MODE_REPLAY_SELECT = 0x35,
    JOB_MODE_SOUND_MODE_EXECUTE = 0x38,
    JOB_MODE_VERSUS_SPLASH = 0x41
};

// Determines UI, player control, and other features in battle.
enum GGXXACPR_GameModeFeatureFlags {
    GAME_MODE_FEATURE_FLAGS_NONE = 0x0,
    GAME_MODE_FEATURE_FLAGS_P1_CONTROL = 0x1,
    GAME_MODE_FEATURE_FLAGS_P2_CONTROL = 0x2,
    GAME_MODE_FEATURE_FLAGS_TRAINING = 0x0100,
    GAME_MODE_FEATURE_FLAGS_SURVIVAL = 0x0200,
    GAME_MODE_FEATURE_FLAGS_DEMO = 0x0400,
    GAME_MODE_FEATURE_FLAGS_VERSUS = 0x800,
    GAME_MODE_FEATURE_FLAGS_VERSUS_CPU = 0x2000,
    GAME_MODE_FEATURE_FLAGS_STORY = 0x4000,
    GAME_MODE_FEATURE_FLAGS_MISSION = 0x8000,
    GAME_MODE_FEATURE_FLAGS_WATCH = 0x10000,
    GAME_MODE_FEATURE_FLAGS_FREE_PLAY = 0x20000,
    GAME_MODE_FEATURE_FLAGS_TEAM_VERSUS = 0x40000,
};

// Character / entity group ids
enum GGXXACPR_EntityId {
    ENTITY_ID_NONE = 0,
    ENTITY_ID_SOL = 1,
    ENTITY_ID_KY = 2,
    ENTITY_ID_MAY = 3,
    ENTITY_ID_MILLIA = 4,
    ENTITY_ID_AXL = 5,
    ENTITY_ID_POTEMKIN = 6,
    ENTITY_ID_CHIPP = 7,
    ENTITY_ID_EDDIE = 8,
    ENTITY_ID_BAIKEN = 9,
    ENTITY_ID_FAUST = 10,
    ENTITY_ID_TESTAMENT = 11,
    ENTITY_ID_JAM = 12,
    ENTITY_ID_ANJI = 13,
    ENTITY_ID_JOHNNY = 14,
    ENTITY_ID_VENOM = 15,
    ENTITY_ID_DIZZY = 16,
    ENTITY_ID_SLAYER = 17,
    ENTITY_ID_I_NO = 18,
    ENTITY_ID_ZAPPA = 19,
    ENTITY_ID_BRIDGET = 20,
    ENTITY_ID_ROBO_KY = 21,
    ENTITY_ID_ABA = 22,
    ENTITY_ID_ORDER_SOL = 23,
    ENTITY_ID_KLIFF = 24,
    ENTITY_ID_JUSTICE = 25,
    ENTITY_ID_DIZZY_ENTITY_1 = 0x20, // Scythe / Ice Spike
    ENTITY_ID_MILLIA_ENTITY_1 = 0x21, // Everything but hair pin and IK
    ENTITY_ID_VENOM_ENTITY_1 = 0x22, // Normal balls
    ENTITY_ID_EDDIE_ENTITY_SHADOW = 0x23,
    ENTITY_ID_KY_ENTITY = 0x29,
    ENTITY_ID_FAUST_ENTITY_1 = 0x2B, // Items except for the minis
    ENTITY_ID_SOL_ENTITY = 0x2C,
    ENTITY_ID_MILLIA_ENTITY_2 = 0x2E, // Hair pin / IK
    ENTITY_ID_JAM_ENTITY = 0x2F,
    ENTITY_ID_VENOM_ENTITY_2 = 0x31, // Charged ball
    ENTITY_ID_VENOM_ENTITY_3 = 0x32, // Dark Angel / IK
    ENTITY_ID_MILLIA_ENTITY_3 = 0x34, // Main IK entity
    ENTITY_ID_MAY_ENTITY_INSTANT_KILL_1 = 0x37,  // Jellyfish pirates
    ENTITY_ID_BAIKEN_ENTITY = 0x39,
    ENTITY_ID_BAIKEN_ENTITY_INSTANT_KILL = 0x3A,
    ENTITY_ID_CHIPP_ENTITY = 0x3B,
    ENTITY_ID_POTEMKIN_ENTITY = 0x3D,
    ENTITY_ID_JOHNNY_ENTITY = 0x3F,
    ENTITY_ID_FAUST_ENTITY_2 = 0x40, // Love explosion / IK
    ENTITY_ID_EDDIE_ENTITY = 0x41,
    ENTITY_ID_DIZZY_ENTITY_2 = 0x43, // Fish / Knife / Bubble / Supers
    ENTITY_ID_MAY_ENTITY_1 = 0x44, // Dolphins
    ENTITY_ID_MAY_ENTITY_2 = 0x46, // Supers
    ENTITY_ID_MAY_ENTITY_3 = 0x48, // Hoop
    ENTITY_ID_FAUST_ENTITY_3 = 0x4A,   // Minis / Love
    ENTITY_ID_MAY_ENTITY_INSTANT_KILL_2 = 0x4B,  // Granny / Mech girl pirates
    ENTITY_ID_ANJI_ENTITY = 0x4C,
    ENTITY_ID_AXL_ENTITY_1 = 0x4D, // Rensen / Kokuu Geki / Super / Counter bandana visual / IK
    ENTITY_ID_ANJI_ENTITY_BUTTERFLY = 0x4E,
    ENTITY_ID_TESTAMENT_ENTITY_1 = 0x4F,   // Curse / Net / Nightmare Circular
    ENTITY_ID_TESTAMENT_ENTITY_2 = 0x50,   // Tree / EXE Beast
    ENTITY_ID_AXL_ENTITY_2 = 0x51, // Unblockable / Counter visual / Shiranami no Homura explosion
    ENTITY_ID_TESTAMENT_ENTITY_CROW = 0x52,
    ENTITY_ID_JOHNNY_ENTITY_COIN = 0x55,
    ENTITY_ID_I_NO_ENTITY = 0x56,
    ENTITY_ID_ZAPPA_ENTITY_RAOU = 0x57,
    ENTITY_ID_SLAYER_ENTITY = 0x58,
    ENTITY_ID_ZAPPA_ENTITY = 0x59,
    ENTITY_ID_ZAPPA_ENTITY_TRIPLETS = 0x5B,
    ENTITY_ID_BRIDGET_ENTITY_1 = 0x5C,
    ENTITY_ID_ZAPPA_ENTITY_SWORD = 0x5D,
    ENTITY_ID_BRIDGET_ENTITY_2 = 0x5E, // KSMH~Stop / Maintenance Disaster / IK
    ENTITY_ID_ZAPPA_ENTITY_DOG = 0x60,
    ENTITY_ID_ROBO_KY_ENTITY_1 = 0x62,  // 6P / 6H / 2D / Rocket / Install explosion
    ENTITY_ID_ROBO_KY_ENTITY_2 = 0x63,  // 2H / Air Rocket Punch / Mini-Robo-Kys in IK
    ENTITY_ID_ABA_ENTITY = 0x64,
    ENTITY_ID_ORDER_SOL_ENTITY_1 = 0x67,
    ENTITY_ID_ORDER_SOL_ENTITY_2 = 0x68,    // Little fire visual on Tyrant Rave (maybe others)
    ENTITY_ID_KLIFF_ENTITY = 0x6C,
    ENTITY_ID_JUSTICE_ENTITY_1 = 0x6E, // Everything but nukes
    ENTITY_ID_JUSTICE_ENTITY_2 = 0x6F, // Nukes
    ENTITY_ID_BURST_ENTITY = 0x84,
};

enum GGXXACPR_StageId {
    STAGE_ID_GREY = 0,  // London alien easter egg
    STAGE_ID_LONDON,
    STAGE_ID_COLONY,
    STAGE_ID_RUSSIA,
    STAGE_ID_CHINA,
    STAGE_ID_MAY_SHIP,
    STAGE_ID_ZEPP,
    STAGE_ID_NIRVANA,
    STAGE_ID_PARIS,
    STAGE_ID_HELL,
    STAGE_ID_GROVE,
    STAGE_ID_VERDANT,
    STAGE_ID_CASTLE,
    STAGE_ID_BABYLON,
    STAGE_ID_PHANTOM_CITY,
    STAGE_ID_UNKNOWN,
    STAGE_ID_FRASCO,
    STAGE_ID_AD2172,
    STAGE_ID_GRAVE,
    STAGE_ID_HEAVEN,
    STAGE_ID_KOREA,

    STAGE_ID_LONDON_RELOAD = 21,
    STAGE_ID_COLONY_RELOAD,
    STAGE_ID_RUSSIA_RELOAD,
    STAGE_ID_CHINA_RELOAD,
    STAGE_ID_MAY_SHIP_RELOAD,
    STAGE_ID_ZEPP_RELOAD,
    STAGE_ID_NIRVANA_RELOAD,
    STAGE_ID_PARIS_RELOAD,
    STAGE_ID_HELL_RELOAD,
    STAGE_ID_GROVE_RELOAD,
    STAGE_ID_VERDANT_RELOAD,
    STAGE_ID_CASTLE_RELOAD,
    STAGE_ID_BABYLON_RELOAD,
    STAGE_ID_PHANTOM_CITY_RELOAD,
    STAGE_ID_UNKNOWN_RELOAD,
    STAGE_ID_FRASCO_RELOAD,
    STAGE_ID_AD2172_RELOAD,
    STAGE_ID_GRAVE_RELOAD,
    STAGE_ID_HEAVEN_RELOAD,
    STAGE_ID_KOREA_RELOAD,

    STAGE_ID_LONDON_SLASH = 41,
    STAGE_ID_COLONY_SLASH,
    STAGE_ID_RUSSIA_SLASH,
    STAGE_ID_CHINA_SLASH,
    STAGE_ID_MAY_SHIP_SLASH,
    STAGE_ID_ZEPP_SLASH,
    STAGE_ID_NIRVANA_SLASH,
    STAGE_ID_PARIS_SLASH,
    STAGE_ID_HELL_SLASH,
    STAGE_ID_GROVE_SLASH,
    STAGE_ID_VERDANT_SLASH,
    STAGE_ID_CASTLE_SLASH,
    STAGE_ID_BABYLON_SLASH,
    STAGE_ID_PHANTOM_CITY_SLASH,
    STAGE_ID_UNKNOWN_SLASH,
    STAGE_ID_FRASCO_SLASH,
    STAGE_ID_AD2172_SLASH,
    STAGE_ID_GRAVE_SLASH,
    STAGE_ID_HEAVEN_SLASH,
    STAGE_ID_KOREA_SLASH,
};

enum GGXXACPR_ColliderId {
    COLLIDER_ID_DUMMY,
    COLLIDER_ID_HIT_BOX,
    COLLIDER_ID_HURT_BOX,
    // Signals the game to use the hitbox of the same array index in `Entity::extraHitboxSet`
    //  instead. This behavior can be seen on Dizzy's knife projectile (421S).
    COLLIDER_ID_USE_EXTRA,
    // Rather than acting like a collider, contains dimensions for
    //  a push box adjustment. Can be seen on Sol's 6H.
    COLLIDER_ID_ADJUST_PUSH,
    // Defines a spawn location for a sub-entity
    COLLIDER_ID_SPAWN,
    COLLIDER_ID_UNKNOWN_6,
};

enum GGXXACPR_ActionState {
    ACTION_STATE_NONE = 0x0,
    ACTION_STATE_IS_ENTITY = 0x1, /* Typically always set unless the entity is despawning */
    ACTION_STATE_COLLIDES_WITH_P2 = 0x2,
    ACTION_STATE_COLLIDES_WITH_P1 = 0x4,
    ACTION_STATE_DRAW_SPRITE = 0x8,
    ACTION_STATE_IS_AIRBORNE = 0x10,
    ACTION_STATE_IS_IN_HITSTUN = 0x20,
    ACTION_STATE_DISABLE_HITBOXES = 0x40,
    ACTION_STATE_DISABLE_HURTBOXES = 0x80, /* Essentially identical to StrikeInvuln but some mechanics/moves use one over the other. */
    ACTION_STATE_KNOCKED_DOWN = 0x100,
    ACTION_STATE_IS_IN_BLOCKSTUN = 0x200,
    ACTION_STATE_IS_CROUCHING = 0x400,
    ACTION_STATE_IS_CORNERED = 0x800,
    ACTION_STATE_LANDING_FLAG = 0x1000,
    ACTION_STATE_IS_AT_SCREEN_LIMIT = 0x2000, /* Bounded by screen edge, but not necessarily cornered */
    ACTION_STATE_PROJECTILE_INVULN = 0x4000, /* A legacy value, that's mainly used in throw animations as a sort of invuln flag. */
    ACTION_STATE_WAKEUP = 0x8000,
    ACTION_STATE_DISABLE_WAKEUP = 0x10000,  /* Set at round end for the KO'd player. */
    ACTION_STATE_STRIKE_INVULN = 0x20000,
    ACTION_STATE_IS_IDLE = 0x40000,
    ACTION_STATE_SUPER_FLASH = 0x80000, /* Set only when the entity is frozen in super flash. */
    ACTION_STATE_NO_COLLISION = 0x100000,
    ACTION_STATE_GRAVITY = 0x200000,
    ACTION_STATE_UNKNOWN_BIT_22 = 0x400000,
    ACTION_STATE_THROW_INVULN = 0x800000,
    ACTION_STATE_UNKNOWN_BIT_24 = 0x1000000,
    ACTION_STATE_UNKNOWN_BIT_25 = 0x2000000,
    ACTION_STATE_UNKNOWN_BIT_26 = 0x4000000,
    ACTION_STATE_UNKNOWN_BIT_27 = 0x8000000,
    ACTION_STATE_UNKNOWN_BIT_28 = 0x10000000,
    ACTION_STATE_UNKNOWN_BIT_29 = 0x20000000,
    ACTION_STATE_IGNORE_RECEIVED_HIT_EFFECTS = 0x40000000, /* Used for Dizzy bubble. Essentially disables hitstop. */
    ACTION_STATE_DESPAWN = 0x80000000, /* Entity is marked for despawn */
};

enum GGXXACPR_AttackState {
    ATTACK_STATE_NONE = 0x0,
    ATTACK_STATE_IS_ATTACK = 0x1,
    ATTACK_STATE_UNKNOWN_BIT_1 = 0x2,
    ATTACK_STATE_UNKNOWN_BIT_2 = 0x4,
    ATTACK_STATE_UNKNOWN_BIT_3 = 0x8,
    ATTACK_STATE_IN_GATLING_WINDOW = 0x10,
    ATTACK_STATE_SPECIAL_CANCEL_OKAY = 0x20,
    ATTACK_STATE_UNKNOWN_BIT_6 = 0x40,
    ATTACK_STATE_UNKNOWN_BIT_7 = 0x80,
    ATTACK_STATE_HOMING_JUMP_OKAY = 0x100,
    ATTACK_STATE_KARA_FD_OKAY = 0x200,
    ATTACK_STATE_NO_SPECIAL_CANCEL = 0x400,
    ATTACK_STATE_IN_RECOVERY = 0x800,
    ATTACK_STATE_HAS_CONNECTED = 0x1000,
    ATTACK_STATE_UNKNOWN_BIT_13 = 0x2000,
    ATTACK_STATE_UNKNOWN_BIT_14 = 0x4000,
    ATTACK_STATE_UNKNOWN_BIT_15 = 0x8000,
    ATTACK_STATE_JUMP_CANCELABLE = 0x10000,
    ATTACK_STATE_HAS_HIT_OPPONENT = 0x20000,
    ATTACK_STATE_UNKNOWN_BIT_18 = 0x40000,
    ATTACK_STATE_UNKNOWN_BIT_19 = 0x80000,
    ATTACK_STATE_UNKNOWN_BIT_20 = 0x100000, /* maybe related to guardpoint */
    ATTACK_STATE_UNKNOWN_BIT_21 = 0x200000,
    ATTACK_STATE_UNKNOWN_BIT_22 = 0x400000,
    ATTACK_STATE_UNKNOWN_BIT_23 = 0x800000
};

// Command state flags. Still unsure on exact usage of these values, so
//  take their names and descriptions with a grain of salt.
//  Command state flags relate to available state transitions
//  excluding gatlings, cancels, and kara cancels.
enum GGXXACPR_CommandState {
    COMMAND_STATE_NONE = 0x0,
    COMMAND_STATE_NO_NEUTRAL = 0x1,
    COMMAND_STATE_NO_FORWARD = 0x2,
    COMMAND_STATE_NO_BACKWARD = 0x4,
    COMMAND_STATE_NO_CROUCH = 0x8,
    COMMAND_STATE_NO_JUMP = 0x10,
    COMMAND_STATE_UNKNOWN_BIT_5 = 0x20,
    COMMAND_STATE_NO_ATTACK = 0x40,
    COMMAND_STATE_UNKNOWN_BIT_7 = 0x80,
    COMMAND_STATE_GUARD = 0x100,
    COMMAND_STATE_UNKNOWN_BIT_9 = 0x200,
    COMMAND_STATE_AIR_DASH = 0x400,
    COMMAND_STATE_UKEMI = 0x800,
    COMMAND_STATE_PREJUMP = 0x1000,
    COMMAND_STATE_DISABLE_THROW = 0x2000,
    COMMAND_STATE_FAUST_CRAWL_FORWARD = 0x4000,
    COMMAND_STATE_FAUST_CRAWL_BACKWARD = 0x8000,
};

enum GGXXACPR_GuardState {
    GUARD_STATE_NONE = 0,
    GUARD_STATE_STAND_BLOCKING = 0x1,
    GUARD_STATE_CROUCH_BLOCKING = 0x2,
    GUARD_STATE_AIR_BLOCKING = 0x4,
    GUARD_STATE_FAULTLESS_DEFENSE = 0x8,
    GUARD_STATE_UNKNOWN_BIT_4 = 0x10,
    GUARD_STATE_UNKNOWN_BIT_5 = 0x20,
    GUARD_STATE_IN_BLOCKSTUN = 0x40,
    GUARD_STATE_UNKNOWN_BIT_7 = 0x80,
    GUARD_STATE_UNKNOWN_BIT_8 = 0x100,
    GUARD_STATE_GUARD_POINT = 0x200,
    GUARD_STATE_ARMOR = 0x400,
    GUARD_STATE_PARRY_1 = 0x800,
    GUARD_STATE_PARRY_2 = 0x1000,
    GUARD_STATE_UNKNOWN_BIT_13 = 0x2000,
    GUARD_STATE_UNKNOWN_BIT_14 = 0x4000,
    GUARD_STATE_UNKNOWN_BIT_15 = 0x8000,
};

enum GGXXACPR_BackgroundState {
    BACKGROUND_STATE_NONE = 0,
    BACKGROUND_STATE_UNKNOWN_BIT_0 = 0x1,
    BACKGROUND_STATE_HUD_OFF = 0x2,
    BACKGROUND_STATE_DEFAULT = 0x4,
    BACKGROUND_STATE_BLACK_BACKGROUND = 0x8,
    BACKGROUND_STATE_POST_FLASH_DIM = 0x10,
    BACKGROUND_STATE_UNKNOWN_BIT_5 = 0x20,
    BACKGROUND_STATE_UNKNOWN_BIT_6 = 0x40,
    BACKGROUND_STATE_UNKNOWN_BIT_7 = 0x80,
    BACKGROUND_STATE_UNKNOWN_BIT_8 = 0x100,
    BACKGROUND_STATE_UNKNOWN_BIT_9 = 0x200,
    BACKGROUND_STATE_UNKNOWN_BIT_10 = 0x400,
    BACKGROUND_STATE_SUPER_FLASH = 0x800,
    BACKGROUND_STATE_UNKNOWN_BIT_12 = 0x1000,
    BACKGROUND_STATE_UNKNOWN_BIT_13 = 0x2000,
    BACKGROUND_STATE_FORCE_BREAK_SUPER_FLASH = 0x4000,
    BACKGROUND_STATE_LIGHTNING = 0x8000,
};

enum GGXXACPR_SpriteRenderState {
    SPRITE_RENDER_STATE_NONE = 0x0,
    SPRITE_RENDER_STATE_IGNORE_UNIVERSAL_Y_OFFSET = 0x8,
    SPRITE_RENDER_STATE_DEAD_ANGLE_OR_FRC_FLASH = 0x200,
    SPRITE_RENDER_STATE_STROBE = 0x400, /* Usually paired with `FIRE` */
    SPRITE_RENDER_STATE_FIRE = 0x1000,
    SPRITE_RENDER_STATE_INSTALL_FLASH = 0x4000,
    SPRITE_RENDER_STATE_INVISIBLE = 0x8000,
    SPRITE_RENDER_STATE_SILHOUETTE = 0x10000,
    SPRITE_RENDER_STATE_MOROHA_MODE_FLASH = 0x80000, /* Also used for Justice's install */
    SPRITE_RENDER_STATE_POISON_FLASH = 0x100000,
    SPRITE_RENDER_STATE_INSTANT_BLOCK_FLASH = 0x800000,
    SPRITE_RENDER_STATE_COUNTER_HIT_FLASH = 0x1000000,
    SPRITE_RENDER_STATE_EASY_FRC_FLASH = 0x4000000, /* Survival mode mechanic */
    SPRITE_RENDER_STATE_LIGHT_GREEN_FLASH = 0x8000000,
    SPRITE_RENDER_STATE_LIGHT_YELLOW_FLASH = 0x10000000,
    SPRITE_RENDER_STATE_LIGHT_BLUE_FLASH = 0x20000000
};

enum GGXXACPR_DrawSpriteAttributes {
    ACPR_DRAW_SPRITE_ALIGN_LEFT = 0x1,
    ACPR_DRAW_SPRITE_ALIGN_X_CENTER = 0x2,
    ACPR_DRAW_SPRITE_ALIGN_TOP = 0x4,
    ACPR_DRAW_SPRITE_ALIGN_Y_CENTER = 0x8,
    ACPR_DRAW_SPRITE_FLIP_TEXTURE_U = 0x10,
    ACPR_DRAW_SPRITE_FLIP_TEXTURE_Y = 0x20,
    ACPR_DRAW_SPRITE_ROTATE_CCW_90 = 0x1000,
    ACPR_DRAW_SPRITE_ROTATE_180 = 0x2000,
};

enum GGXXACPR_RawControllerInput {
    INPUT_NONE = 0x0,
    INPUT_SELECT = 0x1,
    INPUT_L3 = 0x2,
    INPUT_R3 = 0x4,
    INPUT_START = 0x8,
    INPUT_UP = 0x10,
    INPUT_RIGHT = 0x20,
    INPUT_DOWN = 0x40,
    INPUT_LEFT = 0x80,
    INPUT_L2 = 0x100,
    INPUT_R2 = 0x200,
    INPUT_L1 = 0x400,
    INPUT_R1 = 0x800,
    INPUT_TOP_FACE = 0x1000,
    INPUT_RIGHT_FACE = 0x2000,
    INPUT_BOTTOM_FACE = 0x4000,
    INPUT_LEFT_FACE = 0x8000,
};

enum GGXXACPR_Language {
    ACPR_LANG_JPN,
    ACPR_LANG_ENG,
    ACPR_LANG_FRA,
    ACPR_LANG_ITA,
    ACPR_LANG_GER,
    ACPR_LANG_POR,
    ACPR_LANG_SPA,
    ACPR_LANG_KOR
};

/* ========== #STRUCTS ========== */

typedef struct GGXXACPR_LocaleState {
    uint32_t Language;      // enum `GGXXACPR_Language`
    uint32_t Format;        // Unknown, string format maybe
    uint8_t Buffer[0x600];  // String buffer. Size may be larger
} GGXXACPR_LocaleState;
static_assert(sizeof(GGXXACPR_LocaleState) == 0x608);

typedef struct GGXXACPR_FiberData {
    int32_t State;
    int32_t Waiting;
    uint32_t StackSize;
    int32_t UNKNOWN_FIELD(0x0C);
    uint32_t Flags;
    int32_t UNKNOWN_FIELD(0x14);
    char Name[24];
    void* EntryPoint;
    void* Handle;
    PAD(28, 0x38);
} GGXXACPR_FiberData;
static_assert(sizeof(GGXXACPR_FiberData) == 0x54);

typedef struct GGXXACPR_PlayerInput {
    uint32_t InputRaw1;     // enum `GGXXACPR_RawControllerInput`
    uint32_t InputRaw2;     // enum `GGXXACPR_RawControllerInput`
    uint32_t InputPress1;   // enum `GGXXACPR_RawControllerInput`
    uint32_t InputPress2;   // enum `GGXXACPR_RawControllerInput`
    uint32_t InputRelease;  // enum `GGXXACPR_RawControllerInput`
    int32_t SelectCharge;
    int32_t L3Charge;
    int32_t R3Charge;
    int32_t StartCharge;
    int32_t UpCharge;
    int32_t RightCharge;
    int32_t DownCharge;
    int32_t LeftCharge;
    int32_t L2Charge;
    int32_t R2Charge;
    int32_t L1Charge;
    int32_t R1Charge;
    int32_t TopFaceCharge;
    int32_t RightFaceCharge;
    int32_t BottomFaceCharge;
    int32_t LeftFaceCharge;
    PAD(0x40, 0x54);
    uint8_t isActive;
    PAD(3, 0x95);
} GGXXACPR_PlayerInput;
static_assert(sizeof(GGXXACPR_PlayerInput) == 0x98);

typedef struct GGXXACPR_Collider {
    int16_t xOffset;
    int16_t yOffset;
    int16_t width;
    int16_t height;
    uint16_t boxTypeId; /* enum COLLIDER_ID */
    uint16_t boxFlags; /* unknown */
} GGXXACPR_Collider;
static_assert(sizeof(GGXXACPR_Collider) == 0xC );

typedef struct GGXXACPR_ActionHeader {
    uint32_t ActHeaderFlags;
    int16_t lvlFlag;
    uint8_t damage;
    uint8_t flag2;
} GGXXACPR_ActionHeader;

typedef struct GGXXACPR_HitParam {
    int16_t launchVelocityX;
    int16_t launchVelocityY;
    PAD(40, 0x05);
    int16_t counterHitLaunchVelocityX;
    int16_t counterHitLaunchVelocityY;
    PAD(24, 0x30);
    int16_t cleanHitCheckXDist;
    int16_t cleanHitCheckYDist;
    int16_t cleanHitCheckHalfWidth;
    int16_t cleanHitCheckHalfHeight;
    int16_t cleanHitCheckScale;
    uint8_t forceCleanHit;
    PAD(9, 0x53);
} GGXXACPR_HitParam;
/* Struct layout sanity checks */
static_assert(offsetof(GGXXACPR_HitParam, counterHitLaunchVelocityX) == 0x2C);
static_assert(offsetof(GGXXACPR_HitParam, cleanHitCheckXDist) == 0x48);
static_assert(sizeof(GGXXACPR_HitParam) == 0x5C);

typedef struct GGXXACPR_DamageParam {
    int16_t downX;
    int16_t downY;
    int16_t downGravity;
    int16_t downUkemiTime;
    int16_t faintPoint;
    uint16_t UNKNOWN_FIELD(0xA);
    uint32_t downFlag;
    int16_t kezuri; /* chip damage? */
    int16_t damageHosei; /* some scaling thing */
    uint16_t hitSetParamNo;
    uint16_t hitSetParamFlag;
    uint16_t GuardSetParamNo;
    uint16_t GuardSetParamFlag;
    uint8_t dno; // used for hit level, but one less than on wiki
    uint8_t dprob;
    uint8_t gno; // used for hit level, but one less than on wiki
    uint8_t gprob;
    int16_t UNKNOWN_FIELD(0x20);
    int16_t UNKNOWN_FIELD(0x22);
    int16_t UNKNOWN_FIELD(0x24);
    int16_t UNKNOWN_FIELD(0x26);
    int16_t UNKNOWN_FIELD(0x28);
    int16_t UNKNOWN_FIELD(0x2A);
} GGXXACPR_DamageParam;
static_assert(sizeof(GGXXACPR_DamageParam) == 0x2C);

typedef struct GGXXACPR_Screenshake {
    float offset; // Current offset to apply to camera
    uint16_t flags; // First 3 bits determine shake magnitude, 4th bit flips the direction.
    uint16_t timer; // Starts at 1 and counts up until reaching a terminating value (0x7F) in the shake value array.
} GGXXACPR_Screenshake;
static_assert(sizeof(GGXXACPR_Screenshake) == 0x8);
// Covers a region of static game memory that holds camera data
//  as well as some cached calculations related to camera behavior.
//
//  Many values listed are copys of base data and are only used to render certain
//  graphical elements. Manipulating these copies will only change the way elements
//  are rendered for one frame before the value is recopied from the base data.
typedef struct GGXXACPR_Camera {
    float UNKNOWN_FIELD(0x0); // Some sort of zoom scale
    float playerXDist;
    float playerYDist;
    GGXXACPR_Screenshake horizontalScreenshake;
    int32_t cameraXPos_stageCopy;
    int32_t cameraYPos_stageCopy;
    uint32_t cameraWidth_2; // Duplicated for unknown reason
    PAD(4, 0x1C);
    int32_t leftEdgePos;
    int32_t anchorYPosMinus40000; // Unknown use
    uint32_t cameraWidth_3; // Duplicated for unknown reason
    uint32_t cameraHeight;
    float minPlayerYPosFloat;
    int32_t xDiff; // The change in position since the last frame. Data is recorded but unused.
    int32_t yDiff; // The change in position since the last frame. Data is recorded but unused.
    uint32_t cameraWidth_actual; // direct manipulation affects camera size
    int32_t zoomTimer; // Timer that delays rezoom to ideal camera size
    float zoom;
    float zoomBackgroundScale; // direct manipulation affects background render scale
    float zoomSpriteScale; // direct manipulation affects player sprite render scale
    uint32_t cameraFlags;
    PAD(4, 0x54);
    int32_t player1XPos;
    int32_t player2XPos;
    int32_t minPlayerYPos;
    int32_t UNKNOWN_FIELD(0x64); // Something like player 1 screen-space velocity
    int32_t UNKNOWN_FIELD(0x68); // Something like player 2 screen-space velocity
    int32_t UNKNOWN_FIELD(0x6C); // Something about player height
    GGXXACPR_Screenshake verticalScreenshake;
    float playerCenterPointX;
    int32_t cameraXPos_actual; // anchor point on the X-axis is the middle of the camera view
    int32_t cameraYPos_actual; // anchor point on the Y-axis lines up with the ground on the default view. Roughly 1/6th of the screen from the bottom edge.
    PAD(8, 0x84);
    int32_t cameraXPos_spriteCopy;
    int32_t cameraYPos_spriteCopy;
    uint32_t cameraWidth_4; // Duplicated for unknown reason
} GGXXACPR_Camera;

/* Struct layout sanity checks */
static_assert(offsetof(GGXXACPR_Camera, cameraWidth_actual) == 0x40);
static_assert(offsetof(GGXXACPR_Camera, player1XPos) == 0x5C);
static_assert(offsetof(GGXXACPR_Camera, playerCenterPointX) == 0x7C);
static_assert(sizeof(GGXXACPR_Camera) == 0x9C);

/* Character specific union structs */
typedef struct GGXXACPR_CharacterSpecificVariables_Sol {
    int16_t DragonInstallTimer;
    PAD(2, 0x2);
    int16_t DragonInstallSecond; // 2 byte bool
    PAD(26, 0x6);
} GGXXACPR_CharacterSpecificVariables_Sol;
typedef struct GGXXACPR_CharacterSpecificVariables_May {
    uint16_t RestiveRollingLimit;
    /**
     * 1 = Up, 2 = Down, 3 = Forward, 4 = Backward,
     * 5 = Up-forward, 6 = Up-backward, 7 = Down-forward, 8 = Down_backward,
     */
    uint16_t RestiveRollingDirection;
    /**
     * Corresponds to button pressed for 41236X special.
     * P = 0, K = 1, S = 2, H = 3, D = 4
     */
    uint16_t HoopDolphinType;
    PAD(26, 0x6);
} GGXXACPR_CharacterSpecificVariables_May;
typedef struct GGXXACPR_CharacterSpecificVariables_Millia {
    uint16_t LustShakerMash;
    PAD(2, 0x2);
    /**
     * Array of queued directions:
     *  1 = Up, 2 = Down, 3 = Forward, 4 = Backward,
     *  5 = Up-forward, 6 = Up-backward, 7 = Down-forward, 8 = Down_backward,
     */
    uint8_t SecretGardenQueue[4];
    PAD(24, 0x8);
} GGXXACPR_CharacterSpecificVariables_Millia;
typedef struct GGXXACPR_CharacterSpecificVariables_Axl {
    PAD(2, 0x0);
    uint16_t RensenState; // Unknown
    PAD(28, 0x4);
} GGXXACPR_CharacterSpecificVariables_Axl;
typedef struct GGXXACPR_CharacterSpecificVariables_Potemkin {
    uint8_t GiganticBulletInputted;
    PAD(31, 0x1);
} GGXXACPR_CharacterSpecificVariables_Potemkin;
typedef struct GGXXACPR_CharacterSpecificVariables_Chipp {
    int16_t LandBreakActOverride;
    PAD(2, 0x2);
    int16_t InvisibilityTimer;
    // Essentially a 2 byte bool. Game code has the following line for
    //  example when toggling: `FastShuriken = 1 - FastShuriken;`
    int16_t FastShuriken;
    PAD(8, 0x8);
    int16_t InvisibilityLevel;
    PAD(14, 0x12);
} GGXXACPR_CharacterSpecificVariables_Chipp;
typedef struct GGXXACPR_CharacterSpecificVariables_Eddie {
    uint16_t Flags; // Unknown
    PAD(2, 0x2);
    int16_t EddieGauge;
    PAD(1, 0x6);
    uint8_t SummonMode; // 0 = Normal, 1 = Vice
    PAD(24, 0x8);
} GGXXACPR_CharacterSpecificVariables_Eddie;
typedef struct GGXXACPR_CharacterSpecificVariables_Faust {
    PAD(1, 0x0);
    uint8_t DefenderChest; // Chest selected by defender in 236236S
    uint8_t AttackerChest; // Chest selected by attacker in 236236S
    PAD(1, 0x3);
    int32_t DoorXPos; // World coordinate for door exit location
    PAD(24, 0x8);
} GGXXACPR_CharacterSpecificVariables_Faust;
typedef struct GGXXACPR_CharacterSpecificVariables_Testament {
    uint16_t CrowState; // 1 = Tracking Testament, 2 = Perched (idle animation)
    PAD(2, 0x2);
    int16_t DollCount;
    PAD(2, 0x6);
    int16_t CurseAttacks; // Current attack number in the curse sequence
    PAD(2, 0xA);
    int32_t CurseSequence; // The current curse sequence [0,3]
    PAD(16, 0x10);
} GGXXACPR_CharacterSpecificVariables_Testament;
typedef struct GGXXACPR_CharacterSpecificVariables_Jam {
    PAD(1, 0x0);
    uint8_t CardSpecialChainsRemaining;
    PAD(1, 0x2);
    uint8_t RyuujinCards; // 22K
    uint8_t GekirinCards; // 22S
    uint8_t KenroukakuCards; // 22H
    PAD(1, 0x6);
    uint8_t ParryBufferTime; // Buffer window timer on successful parry
    int8_t Parry; // -1 = Parry window, -2 = Successful hitstop, >0 = Cooldown timer
    uint8_t JumpFlagOverride;
    uint8_t UnknownOverride;
    PAD(21, 0xB);
} GGXXACPR_CharacterSpecificVariables_Jam;
typedef struct GGXXACPR_CharacterSpecificVariables_Anji {
    int16_t GuardPointCancelTimer;
    int16_t GuardPointCancelOkay;
    PAD(28, 0x4);
} GGXXACPR_CharacterSpecificVariables_Anji;
typedef struct GGXXACPR_CharacterSpecificVariables_Johnny {
    int16_t MistFinerLevel;
    PAD(2, 0x2);
    uint8_t EXMistFinerTimer;
    uint8_t CoinsUsed;
    PAD(26, 0x6);
} GGXXACPR_CharacterSpecificVariables_Johnny;
typedef struct GGXXACPR_CharacterSpecificVariables_Venom {
    uint8_t BilliardLaunchAngle;
    uint8_t ChargedBilliardTimer;
    uint8_t BilliardHealth; // Health imparted to billiards struck (billiard health determines damage)
    uint8_t BilliardBounceFlags; // Flags imparted to billiards struck determine bounce behavior
    PAD(4, 0x4);
    uint8_t EXObjectBallStocks;
    PAD(23, 0x9);
} GGXXACPR_CharacterSpecificVariables_Venom;
typedef struct GGXXACPR_CharacterSpecificVariables_Dizzy {
    uint8_t FishActQueue[4];
    uint8_t KnifeReleased; // bool
    PAD(3, 0x5);
    uint8_t EXKnifeStandby; // bool
    PAD(1, 0x9);
    uint16_t EXNecroInstallTimer;
    PAD(20, 0xC);
} GGXXACPR_CharacterSpecificVariables_Dizzy;
typedef struct GGXXACPR_CharacterSpecificVariables_Zappa {
    uint8_t Summon; // 0 = Naked, 1 = Sword, 2 = Dog, 3 = Triplets, 4 = Raou
    PAD(1, 0x1);
    int16_t Timer; // Summon rng timer?
    int16_t RaouTimer;
    int16_t ChatterCooldown; // lol
    int16_t DogGatlingLimitReached; // 2 byte bool
    uint16_t DogActionBuffer;
    PAD(3, 0xC);
    uint8_t AirDarknessAnthemUsesRemaining; // Set to 1 upon going airborne
    uint16_t RaouDuration; // RaouTimer is initialized to this value. Referenced by HUD element display code.
    PAD(14, 0x12);
} GGXXACPR_CharacterSpecificVariables_Zappa;
typedef struct GGXXACPR_CharacterSpecificVariables_Bridget {
    int16_t YoYoSpeed;
    uint16_t YoYoFlags;
    uint16_t YoYoSetupTimer;
    PAD(2, 0x6);
    uint16_t RollingMovementUsesRemaining; // Set to 1 upon going airborne
    uint16_t ThrowDirection; // Yo-yo throw direction given in numpad notation [1-9];
    uint16_t Unknown_0xC;
    uint16_t Unknown_0xE;
    int32_t ThrowXVelocity;
    int32_t ThrowYVelocity;
    PAD(8, 0x18);
} GGXXACPR_CharacterSpecificVariables_Bridget;
typedef struct GGXXACPR_CharacterSpecificVariables_RoboKy {
    int16_t HeatGain; // Target heat value is (HeatGain * 100/111) + 3000 and caps at 12000
    PAD(2, 0x2);
    int16_t Heat;
    int16_t RisKyTimer;
    PAD(6, 0x8);
    int16_t RisKyHitboxIndex;
    PAD(16, 0x10);
} GGXXACPR_CharacterSpecificVariables_RoboKy;
typedef struct GGXXACPR_CharacterSpecificVariables_ABA {
    uint16_t InstallMode; // 0 = Normal, 1 = Moroha, 2 = Goku Moroha
    uint16_t BloodBagCount;
    PAD(2, 0x4);
    uint16_t HealthReduction; // Health loss on hit
    uint16_t BloodGauge;
    PAD(4, 0xA);
    uint16_t InstallMode_2; // Mode Duplicate? 0 = Normal, 1 = Moroha, 2 = Goku Moroha
    uint16_t InstallAnimationTimer; // Timer used for background fade effect
    PAD(14, 0x12);
} GGXXACPR_CharacterSpecificVariables_ABA;
typedef struct GGXXACPR_CharacterSpecificVariables_OrderSol {
    uint16_t ChargeLevelFreezeTimer; // `ChargeLevelActual` Does not update while the value is non zero
    PAD(6, 0x2);
    uint16_t ChargeLevelActual; // Red part
    uint16_t DragonInstallFollowUp; // 1 = Good, 2 = Failed;
    uint16_t DragonInstallTimer; // Timer for traditional Dragon Install boss mechanic
    uint16_t ChargeLevelTarget; // Yellow part
    PAD(16, 0x10);
} GGXXACPR_CharacterSpecificVariables_OrderSol;
typedef struct GGXXACPR_CharacterSpecificVariables_Kliff {
    uint16_t ScaleRipperTimer;
    PAD(30, 0x2);
} GGXXACPR_CharacterSpecificVariables_Kliff;
typedef struct GGXXACPR_CharacterSpecificVariables_Justice {
    uint16_t InstallTimer;
    uint16_t AirMichaelSwordUsesRemaining; // Set to 1 upon being airborne
    PAD(28, 0x4);
} GGXXACPR_CharacterSpecificVariables_Justice;

static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Sol));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_May));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Millia));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Axl));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Potemkin));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Chipp));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Eddie));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Faust));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Testament));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Jam));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Anji));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Johnny));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Venom));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Dizzy));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Zappa));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Bridget));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_RoboKy));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_ABA));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_OrderSol));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Kliff));
static_assert(0x20 == sizeof(GGXXACPR_CharacterSpecificVariables_Justice));

typedef union GGXXACPR_CharacterSpecificVariables {
    uint8_t RawData[32];
    GGXXACPR_CharacterSpecificVariables_Sol Sol;
    GGXXACPR_CharacterSpecificVariables_May May;
    GGXXACPR_CharacterSpecificVariables_Millia Millia;
    GGXXACPR_CharacterSpecificVariables_Axl Axl;
    GGXXACPR_CharacterSpecificVariables_Potemkin Potemkin;
    GGXXACPR_CharacterSpecificVariables_Chipp Chipp;
    GGXXACPR_CharacterSpecificVariables_Eddie Eddie;
    GGXXACPR_CharacterSpecificVariables_Faust Faust;
    GGXXACPR_CharacterSpecificVariables_Testament Testament;
    GGXXACPR_CharacterSpecificVariables_Jam Jam;
    GGXXACPR_CharacterSpecificVariables_Anji Anji;
    GGXXACPR_CharacterSpecificVariables_Johnny Johnny;
    GGXXACPR_CharacterSpecificVariables_Venom Venom;
    GGXXACPR_CharacterSpecificVariables_Dizzy Dizzy;
    GGXXACPR_CharacterSpecificVariables_Zappa Zappa;
    GGXXACPR_CharacterSpecificVariables_Bridget Bridget;
    GGXXACPR_CharacterSpecificVariables_RoboKy RoboKy;
    GGXXACPR_CharacterSpecificVariables_ABA ABA;
    GGXXACPR_CharacterSpecificVariables_OrderSol OrderSol;
    GGXXACPR_CharacterSpecificVariables_Kliff Kliff;
    GGXXACPR_CharacterSpecificVariables_Justice Justice;
} GGXXACPR_CharacterSpecificVariables;

// A sub-struct that contains data exclusive to player entities (as opposed to more generic entity data).
typedef struct GGXXACPR_PlayerData {
    // Range: [0, 10000]
    uint16_t tension;
    uint8_t staggerShakeTime;
    uint8_t backStepInvalid;
    uint16_t currentGlobalProration;
    int16_t negativePenaltyTimer;
    int16_t counterRedTime; /* Timer for the red flash effect on players that have just taken a counter hit. */
    uint16_t homingJumpFlag;
    int32_t homingJumpTimer;
    int32_t runInertia;
    int32_t offX;
    int16_t throwProtectionTimer;
    int16_t guardBar;
    int16_t guardTimer;
    int16_t untechTimer;
    uint16_t homingJumpDir;
    uint8_t reqHit;
    uint8_t hitDispTime;
    float hitDispIn;
    uint8_t inertiaFlag;
    uint8_t whiteOutTime;
    uint8_t invulnCounter;
    uint8_t airDashRecoveryTimer;
    uint8_t inertiaTime;
    uint8_t downBoundDamage;
    PAD(2, 0x2E);
    int16_t tensionGainPenaltyTimer;
    uint8_t frcTime;
    uint8_t frcLockoutTimer;
    int32_t UNKNOWN_FIELD(0x34);
    int16_t UNKNOWN_FIELD(0x38);
    int16_t UNKNOWN_FIELD(0x3A);
    uint8_t UNKNOWN_FIELD(0x3C);
    uint8_t instantBlockTimer;
    int16_t instantBlockLockoutTimer;

    /* OFFSET 0x40 -------------------------------------------------- */

    uint32_t getChainDataTableFunc;
    uint32_t enemyPtr;
    int16_t punchKey;
    int16_t kickKey;
    int16_t slashKey;
    int16_t heavySlashKey;
    int16_t dustKey;
    int16_t fuckinKey;
    int16_t posResetKey;
    int16_t pauseKey;
    int16_t recPlayerKey;
    int16_t recEnemyKey;
    int16_t playMemoryKey;
    int16_t switchKey;
    int16_t enemyWalkKey;
    int16_t enemyJumpKey;
    int16_t punchAndKickKey;
    int16_t punchAndDustKey;
    int16_t pksKey;
    int16_t pksshKey;
    int16_t lifeLength;
    int16_t lifeLengthDamage;
    uint8_t jumpCount;
    uint8_t airDashCount;
    int16_t jumpFlags;
    int16_t chainFlag1;
    int16_t chainFlag2;
    uint32_t rendaFlag;
    int16_t dizzyBuildup;
    int16_t dizzyDuration;

    /* OFFSET 0x80 -------------------------------------------------- */

    uint32_t exCommandFunc;
    uint8_t jumpDirection;
    uint8_t cpu;
    uint8_t sousaiTime;
    uint8_t dizzyThreshold; /* AKA FaintMax in debug symbols */
    GGXXACPR_CharacterSpecificVariables characterSpecific;
    PAD(4, 0x9C);
    uint32_t hitstunFlags; /* SlidingKD, wall bounce, etc */
    uint32_t UNKNOWN_FIELD(0xB0); /* RE note: Child?*/
    uint32_t atkHitInterrupt;
    uint32_t atkGuardInterrupt;
    uint32_t damageInterrupt;

    /* OFFSET 0xC0 -------------------------------------------------- */

    uint32_t landInterrupt;
    uint32_t actChangeInterrupt;
    uint32_t guardFunc;
    uint32_t getUpInterrupt;
    uint32_t guardCheckInterrupt;
    uint32_t posByEnemyAddr;
    uint8_t overKillFlag;
    uint8_t objFlag;
    uint8_t dustTime;
    uint8_t afterImageIndex;
    int32_t keyHook;
    int16_t healthVisual;
    int16_t tensionPulse;
    int16_t tensionSideTime;
    int16_t tensionPassivityCounter;
    int32_t lastPosX;
    int16_t tensionBar;
    uint8_t tShand;
    uint8_t pushTime;
    uint8_t attackDown;
    uint8_t defenseDown;
    uint8_t jumpSeal;
    uint8_t artsSeal;
    uint8_t poison;
    uint8_t mistTime; /* Baccus Sigh */
    int16_t comboTime;
    int16_t burstMeter;
    uint8_t UNKNOWN_FIELD(0xFA); /* RE Notes: something about throw */
    uint8_t throwCounterThing;
    uint8_t UNKNOWN_FIELD(0xFC); /* RE Notes: relates to blue burst */
    uint8_t UNKNOWN_FIELD(0xFD);
    int16_t comboCounter;

    /* OFFSET 0x100 -------------------------------------------------- */

    int16_t comboCounterVisual;
    uint8_t UNKNOWN_FIELD(0x102); /* RE Note: some counter relates to light green flash effect */
    uint8_t UNKNOWN_FIELD(0x103);
    uint8_t allCounter;
    uint8_t punchSeal;
    uint8_t kickSeal;
    uint8_t slashSeal;
    uint8_t heavySlashSeal;
    uint8_t dustSeal;
    uint8_t UNKNOWN_FIELD(0x10A); /* RE Note: relates to IB/SB */
    uint8_t slashBackActiveWindow;
    uint8_t slashBackGuardLockoutTimer;
    uint8_t slashBackGuardstunTimer;
    uint8_t slashBackLockoutOnWakeupTimer;
    uint8_t slashBackFollowupTimer;
    int16_t throwTechFlags;
    uint8_t UNKNOWN_FIELD(0x112); /* RE Note: Resets beginning of act_update */
    uint8_t UNKNOWN_FIELD(0x113); /* RE Note: a count down timer */
    int32_t throwTechColorCounter;
    int32_t UNKNOWN_FIELD(0x118); /* RE Note: Resets beginning of act_update */
    int32_t cleanHitCounter;
    int32_t UNKNOWN_FIELD(0x120);
    int32_t UNKNOWN_FIELD(0x124);
    int32_t UNKNOWN_FIELD(0x128);
    int32_t UNKNOWN_FIELD(0x12C);
    int32_t UNKNOWN_FIELD(0x130);
    int32_t UNKNOWN_FIELD(0x134);
    int32_t tensionCopy;
    int32_t UNKNOWN_FIELD(0x13C); /* RE Note: Tension thing (relates to expenditure) */

    /* OFFSET 0x140 -------------------------------------------------- */

    int32_t tensionRefill; /* Training */
    int32_t UNKNOWN_FIELD(0x144);
} GGXXACPR_PlayerData;

/* Struct layout sanity checks */
static_assert(offsetof(GGXXACPR_PlayerData, getChainDataTableFunc) == 0x40);
static_assert(offsetof(GGXXACPR_PlayerData, exCommandFunc) == 0x80);
static_assert(offsetof(GGXXACPR_PlayerData, landInterrupt) == 0xC0);
static_assert(offsetof(GGXXACPR_PlayerData, comboCounterVisual) == 0x100);
static_assert(offsetof(GGXXACPR_PlayerData, tensionRefill) == 0x140);
static_assert(sizeof(GGXXACPR_PlayerData) == 0x148);


/*
* Represents Players, projectiles, summons, and graphical effects.
*   Essentially anything that is represented by a sprite.
*/
typedef struct GGXXACPR_Entity GGXXACPR_Entity;
typedef struct GGXXACPR_Entity {
    uint16_t id; /* enum ENTITY_ID*/
    uint8_t bIsFacingRight;
    uint8_t bIsLeftSide;
    // Together with the `Next` pointer forms a doubly-linked list. There is
    //  one linked list for projectiles/summons and another for graphical entities.
    //  Each linked-listed has a dummy root entity (unsure about graphic entities).
    //  These values are nullptr for player entities.
    struct GGXXACPR_Entity* prevPtr;
    // Together with the `Prev` pointer forms a doubly-linked list. There is
    //  one linked list for projectiles/summons and another for graphical entities.
    //  Each linked-listed has a dummy root entity (unsure about graphic entities).
    //  These values are nullptr for player entities.
    struct GGXXACPR_Entity* nextPtr;
    uint32_t actionState; /* enum ACTION_STATE */
    uint16_t bufferedActionId;
    uint16_t bufferFlag; /* bitfield */
    uint16_t bufferedActionIdTemp;
    uint16_t bufferFlagTemp;
    uint16_t actId; /* Current action id. */
    uint16_t localId;
    uint16_t actTimer; /* Frame counter for the current action. */
    uint16_t health; /* [0 - 460] */
    GGXXACPR_Entity* parentEntity;
    uint16_t actionIdSv;
    PAD(1, 0x26);
    uint8_t playerIndex; /* P1 = 0, P2 = 1 */
    uint16_t parentFlag;
    uint16_t guardState; /* enum GUARD_STATE */
    // Contains additional player entity data. Always nullptr on non-player entities.
    GGXXACPR_PlayerData* playerEntityDataPtr;
    uint32_t graphicFunction;
    uint32_t attackState; /* enum ATTACK_STATE */
    uint32_t commandFlags; /* enum COMMAND_STATE */
    uint32_t cellTop;

    /* OFFSET 0x40 -------------------------------------------------- */

    uint16_t cellNo;
    uint16_t lastCellNo;
    uint16_t imageNo;
    uint16_t lastImageNo;
    uint32_t gclTop;
    int16_t coreX;
    int16_t coreY;
    int16_t scale;
    int16_t scaleY;
    GGXXACPR_Collider* colliderSetPtr; /* pointer to a Collider struct array. */
    GGXXACPR_Collider* extraColliderSetPtr; /* pointer to a Collider struct array. */
    uint8_t boxFlag;
    uint8_t hitboxFlag;
    uint8_t hurtboxFlag;
    PAD(1, 0x5F);
    uint32_t tensionAddByHit;
    struct GGXXACPR_Entity* attackTargetPtr;
    struct GGXXACPR_Entity* hitColliPtr;
    struct GGXXACPR_Entity* damColliPtr;
    int32_t imageW;
    int32_t imageH;
    uint8_t colliCnt;
    uint8_t colliNo;
    int16_t priority2;
    int16_t UNKNOWN_FIELD(0x7C); /* RE note: maybe something to do with the after image trail */
    int16_t UNKNOWN_FIELD(0x7E); /* RE note: maybe something to do with the after image trail */

    /* OFFSET 0x80 -------------------------------------------------- */

    int16_t displayVar1;
    int16_t displayVar2;
    uint8_t boxCount; /* length of `hitboxSetPtr` and `extraHitboxSetPtr` arrays */
    int8_t hitboxSetIterationVar;
    PAD(2, 0x86);
    uint32_t hitParamPtr;
    uint32_t damParamPtr;
    uint32_t colliHitInterrupt;
    uint32_t paletteProcedurePtr;
    uint8_t paletteId;
    uint8_t localPalette;
    uint8_t paletteFlag;
    uint8_t fadVal;
    int16_t spriteIdTop;
    int16_t fbSpriteIdTop;
    int16_t spriteIdMax;
    int16_t spriteId;
    uint32_t sphAddr;
    int16_t transMode;
    uint8_t spriteIdIdx;
    uint8_t texSizeMax;
    uint32_t spriteFlag;
    int32_t xPos; /* In world units */
    int32_t yPos; /* In world units */
    int32_t xVelocity; /* world units per frame */
    int32_t yVelocity; /* world units per frame */

    /* OFFSET 0xC0 -------------------------------------------------- */

    int32_t speedsvX;
    int32_t speedsvY;
    int32_t hitBackX;
    int32_t rHitBackX;
    int32_t adBackX;
    int32_t gravity;
    int16_t parentX;
    int16_t parentY;
    uint16_t angle;
    int16_t drawPriority; /* Higher values draw first, lower values draw over higher values (i.e. Z position). */
    uint32_t instTable; /* Pointer to an array of ActionHeader struct pointers */
    GGXXACPR_ActionHeader actHeader;
    GGXXACPR_ActionHeader* actHeaderPtr;
    uint32_t instAddr;
    uint16_t instSt;
    uint16_t instVal;
    uint16_t animationNo;
    uint16_t animationTime;
    uint8_t animationRate;
    uint8_t hitstopTime; /* Hitstop duration timer. */
    uint8_t animationTimeMax;
    int8_t mark; /* move-specific variable. Used to denote active windows of command grabs, some counter stance moves, etc. */

    /* OFFSET 0x100 -------------------------------------------------- */

    uint8_t trans;
    uint8_t trans2;
    uint16_t dashFlags;
    uint16_t airDashFlags;
    uint16_t airDashFlags2;
    PAD(24, 0x108);
    uint32_t throwFlags; /* bitfield */
    PAD(12, 0x124);
} GGXXACPR_Entity;

/* Struct layout sanity checks */
static_assert(offsetof(GGXXACPR_Entity, cellNo) == 0x40);
static_assert(offsetof(GGXXACPR_Entity, displayVar1) == 0x80);
static_assert(offsetof(GGXXACPR_Entity, speedsvX) == 0xC0);
static_assert(offsetof(GGXXACPR_Entity, trans) == 0x100);
static_assert(sizeof(GGXXACPR_Entity) == 0x130);

enum GGXXACPR_TrainingState {
    ACPR_TRAINING_STATE_DEFAULT,
    ACPR_TRAINING_STATE_SWITCH,
    ACPR_TRAINING_STATE_RECORDING_STANDBY,
    ACPR_TRAINING_STATE_RECORDING_RECORD,
    ACPR_TRAINING_STATE_RECORDING_PLAYBACK,
    ACPR_TRAINING_STATE_PRACTICE_UNUSED,
};
/**
 *  \brief Training mode dummy reversal recording struct
 */
typedef struct GGXXACPR_ReversalRecordingState {
    uint8_t UNKNOWN_FIELD(0x0);
    uint8_t UNKNOWN_FIELD(0x1);
    uint16_t PlaybackTimer;
    int32_t* Recording;
    int32_t InstantStartIndex;
    int32_t RecordingSize;
} GGXXACPR_ReversalRecordingState;
static_assert(sizeof(GGXXACPR_ReversalRecordingState) == 0x10);
/**
 *  \brief Training mode dummy recording struct
 */
typedef struct GGXXACPR_DummyRecordingState {
    uint16_t State; // Enum GGXXACPR_TrainingState
    uint16_t PlaybackTimer;
    int16_t UNKNOWN_FIELD(0x4);
    int16_t RNG;
    int32_t Recordings[4500][4];
    int32_t UNKNOWN_FIELD(0x11948); // Current slot?
    int32_t InstantStartIndices[4];
    int32_t RecordingSizes[4];
    int32_t UNKNOWN_FIELD(0x1196C);   // Recording ptr?
    int32_t UNKNOWN_FIELD(0x11970);   // starting point?
    int32_t UNKNOWN_FIELD(0x11974);   // recording size?
    int32_t UNKNOWN_FIELD(0x11978);
    struct GGXXACPR_ReversalRecordingState ReversalRecordingStates[2];
    uint32_t InputArray[4][2]; // Enum GGXXACPR_RawControllerInput
} GGXXACPR_DummyRecordingState;
static_assert(offsetof(GGXXACPR_DummyRecordingState, UNKNOWN_FIELD(0x11948)) == 0x11948);
static_assert(offsetof(GGXXACPR_DummyRecordingState, UNKNOWN_FIELD(0x11978)) == 0x11978);
static_assert(sizeof(GGXXACPR_DummyRecordingState) == (0x6bb6d0 - 0x6a9d14));

#if defined(D3DCOLOR_DEFINED) && defined(D3DVECTOR_DEFINED)
    typedef D3DVECTOR GGXXACPR_Vector;
    typedef D3DCOLOR GGXXACPR_Color;
#else
    typedef struct GGXXACPR_Vector {
        float x, y, z;
    } GGXXACPR_Vector;
    // Format is A8R8G8B8
    typedef uint32_t GGXXACPR_Color;
#endif

typedef struct GGXXACPR_ColorVertex {
    GGXXACPR_Vector Vector;
    GGXXACPR_Color Color;
} GGXXACPR_ColorVertex;

/**
 *  A parameter struct used for drawing sprites.
 */
typedef struct GGXXACPR_DrawSpriteParams {
    int32_t spriteId;
    float x, y, z;
    float zm_x, zm_y;
    float u0, v0, u1, v1;
    int32_t angle;
    float alpha;    // transparency
    int16_t listType;
    uint16_t attribute; // see enum GGXXACPR_DrawSpriteAttributes
    int32_t base_color;
    int32_t offset_color;
    int32_t size_x;
    int32_t size_y;
    int32_t texture;
} GGXXACPR_DrawSpriteParams;
/* Struct layout sanity checks */
static_assert(offsetof(GGXXACPR_DrawSpriteParams, z) == 0x0C);
static_assert(offsetof(GGXXACPR_DrawSpriteParams, v0) == 0x1C);
static_assert(offsetof(GGXXACPR_DrawSpriteParams, v1) == 0x24);
static_assert(sizeof(GGXXACPR_DrawSpriteParams) == 0x48);

#undef UNKNOWN_FIELD
#undef PAD

#endif
