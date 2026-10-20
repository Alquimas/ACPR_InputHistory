#ifndef GEARLOADER_H

#if !defined(_WIN32) || defined(_WIN64)
    #error "Mods should be built targeting 32-bit windows (x86) (same target as +R)"
#endif

#define GEARLOADER_H


#define GEARLOADER_VERSION "0.1.0"
#define GEARLOADER_VERSION_NUM 0x000100
#define GEARLOADER_VERSION_SEM_VER {0,1,0}

#if __cplusplus
    #include <cstdint>
#else
    #include <stdint.h>
#endif


typedef struct SemanticVersion {
    uint32_t major;
    uint32_t minor;
    uint32_t patchNum;
} SemanticVersion;

inline int compare(const SemanticVersion* a, const SemanticVersion* b) {
    if (a->major - b->major != 0) {
        return a->major - b->major;
    }
    if (a->minor - b->minor != 0) {
        return a->minor - b->minor;
    }
    return a->patchNum - b->patchNum;
}

typedef enum GearLoaderLogLevel {
    GEARLOADER_LOG_LEVEL_DEBUG,
    GEARLOADER_LOG_LEVEL_INFO,
    GEARLOADER_LOG_LEVEL_WARN,
    GEARLOADER_LOG_LEVEL_ERR,
    GEARLOADER_LOG_LEVEL_VERBOSE
} GearLoaderLogLevel;

/**
 *  \brief  Forward this incomplete type to API calls to provide calling context to the mod loader.
 */
typedef struct GearLoaderContext GearLoaderContext;

typedef struct GearLoaderApi {
    //! The size of this struct
    uint32_t size;
    //! The version of this struct's layout
    uint32_t version;

    /**
     *  \brief Retrieves an exported API of another loaded mod.
     * 
     *  To ensure the requested mod is installed and loaded, list it in your mod's `config.json` file.
     * 
     *  \param ctx A context pointer owned and used by the mod loader. Simply forward this from the
     *      `Init` function's parameter of the same name.
     *  \param name The name of the requested API.
     *  \param versionConstraint A constraint on the API version to retrieve. Takes the form of
     *      "[operator][semanticVersion]" where operator may be any of ["<", "<=", "=", ">=", ">"].
     *      Examples: ">=0.1.0" or "1.0.0"
     *  \param pApi A pointer to a pointer variable that receives the API pointer.
     *  \param retrievedVersion A pointer to a `SemanticVersion` structure that receives version
     *      information of the received version.
     *  \return An error code if an error occurred, otherwise `0`.
     */
    int32_t __stdcall (*RetrieveModApi)(
        GearLoaderContext* ctx,
        const char* name,
        const char* versionConstraint,
        const void** pApi,
        SemanticVersion* retrievedVersion);

    /**
     *  \brief Registers an API with the mod loader.
     * 
     *  APIs are given by a generic pointer and expected to be reinterpreted by other mods that
     *      retrieve it. The registering mod still owns the underlying API struct and is expected
     *      to keep it alive for the duration of the application's lifetime.
     *      A mod may register multiple APIs differing by name and/or version.
     *      Registering a mod with the same name and version will not override the previous API
     *      and will result in an error.
     * 
     *  \param ctx A context pointer owned and used by the mod loader. Simply forward this from the
     *      `Init` function's parameter of the same name.
     *  \param api A pointer the the API struct to be registered.
     *  \param name The name of the registered API. This same name will need to be passed to `RetrieveModApi`.
     *  \param version The version of the registered API. Used when resolving the version constraint passed to `RetrieveModApi`.
     *  \return An error code if an error occurred, otherwise `0`.
     */
    int32_t __stdcall (*RegisterApi)(
        GearLoaderContext* ctx,
        const void* api,
        const char* name,
        SemanticVersion version);

    /**
     *  \brief Logs the given string to the `GearLoader.log` file.
     * 
     *  Logs will be prefixed with a timestamp, a [DEBUG] label, and a [mod-name] label where
     *      "mod-name" is the calling mod's name provided in its `config.json` file.
     * 
     *  \param ctx A context pointer owned and used by the mod loader. Simply forward this from the
     *      `Init` function's parameter of the same name.
     *  \param logLevel Enum `GearLoaderLogLevel`. Verbose calls only get logged when running with the
     *      `-GearLoaderVerbose` launch option.
     *  \param str The string to be logged.
     *  \return An error code if an error occurred, otherwise `0`.
     */
    uint32_t __stdcall (*Log)(
        GearLoaderContext* ctx,
        uint32_t logLevel,
        const char* str);
} GearLoaderApi;

#ifdef __cplusplus
    #define GEARLOADER_EXPORT extern "C" __declspec(dllexport)
#else
    #define GEARLOADER_EXPORT __declspec(dllexport)
#endif
#define GEARLOADER_CALL __cdecl

//! Mods may export an "Init" function that follows this signature
typedef void (GEARLOADER_CALL *ModInitFunc)(GearLoaderContext* ctx, GearLoaderApi* api);

#endif
