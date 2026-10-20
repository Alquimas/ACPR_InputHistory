#ifndef GEARLOADER_HPP
#define GEARLOADER_HPP

#include "gearLoader_c.h"
#include <string>


inline bool operator==(const SemanticVersion& a, const SemanticVersion& b) {
    return a.major == b.major &&
        a.minor == b.minor &&
        a.patchNum == b.patchNum;
}
inline bool operator>(const SemanticVersion& a, const SemanticVersion& b) {
    return a.major > b.major ||
        (a.major == b.major && a.minor > b.minor) ||
        (a.major == b.major && a.minor == b.minor && a.patchNum > b.patchNum);
}
inline bool operator!=(const SemanticVersion& a, const SemanticVersion& b) { return !(a == b); }
inline bool operator<(const SemanticVersion& a, const SemanticVersion& b) { return b > a; }
inline bool operator>=(const SemanticVersion& a, const SemanticVersion& b) { return !(b > a); }
inline bool operator<=(const SemanticVersion& a, const SemanticVersion& b) { return !(a > b); }

namespace GearLoader {
    enum class LogLevel {
        DEBUG = GEARLOADER_LOG_LEVEL_DEBUG,
        INFO = GEARLOADER_LOG_LEVEL_INFO,
        WARN = GEARLOADER_LOG_LEVEL_WARN,
        ERR = GEARLOADER_LOG_LEVEL_ERR,
        VERBOSE = GEARLOADER_LOG_LEVEL_VERBOSE
    };

    /**
     *  \brief A C++ wrapper around the GearLoader's api struct pointer.
     * 
     *  Handles version checks and type conversions.
     */
    class Api {
    public:
        Api(GearLoaderApi* c_api, GearLoaderContext* ctx) : base(c_api), _ctx(ctx) { }
        /**
         *  Returns true if there is a difference in major version number between
         *      actual and expected GearLoader versions.
         */
        bool VersionError() {
            return (GEARLOADER_VERSION_NUM & 0xFF0000) != (base->version & 0xFF0000);
        }
        /**
         *  \brief Retrieves an exported API of another loaded mod.
         * 
         *  To ensure the requested mod is installed and loaded, list it in your mod's `config.json` file.
         * 
         *  \param name The name of the requested API.
         *  \param versionConstraint A constraint on the API version to retrieve. Takes the form of
         *      "[operator][semanticVersion]" where operator may be any of ["<", "<=", "=", ">=", ">"].
         *      Examples: ">=0.1.0" or "1.0.0"
         *  \param pApi A pointer to a pointer variable that receives the API pointer.
         *  \param retrievedVersion A pointer to a `SemanticVersion` structure that receives version
         *      information of the received version.
         *  \return An error code if an error occurred, otherwise `0`
         */
        template<typename ApiType>
        int RetrieveModApi(std::string name,
                           std::string versionConstraint,
                           const ApiType** pApi,
                           SemanticVersion* retrievedVersion) {
            if (VersionError()) return 3;

            const void* retApi;
            int result = base->RetrieveModApi(
                _ctx,
                name.c_str(),
                versionConstraint.c_str(),
                &retApi,
                retrievedVersion
            );
            *pApi = reinterpret_cast<const ApiType*>(retApi);
            return result;
        }
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
         *  \param api A pointer the the API struct to be registered.
         *  \param name The name of the registered API. This same name will need to be passed to `RetrieveModApi`.
         *  \param version The version of the registered API. Used when resolving the version constraint passed to `RetrieveModApi`.
         *  \return An error code if an error occurred, otherwise `0`.
         */
        int RegisterApi (const void* api,
                         std::string name,
                         SemanticVersion version) {
            if (VersionError()) return 3;
            return base->RegisterApi(_ctx, api, name.c_str(), version);
        }
        /**
         *  \brief Logs the given string to the `GearLoader.log` file.
         * 
         *  Logs will be prefixed with a timestamp, a [DEBUG] label, and a [mod-name] label where
         *      "mod-name" is the calling mod's name provided in its `config.json` file.
         * 
         *  \param str The string to be logged.
         *  \return An error code if an error occurred, otherwise `0`.
         */
        int Log(LogLevel logLevel, std::string str) {
            if (VersionError()) return 3;
            return base->Log(_ctx, static_cast<uint32_t>(logLevel), str.c_str());
        }
    private:
        GearLoaderApi* base;
        GearLoaderContext* _ctx;
    };
}

#endif
