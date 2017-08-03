#pragma once
#include "common.hpp"

#define CLEANUP_ENABLED_DEFAULT true
#define CLEANUP_DELAY_DEFAULT 300

typedef std::tuple<object, clock_t, bool> killed_map_type;

class uksf_ai_cleanup : public singleton<uksf_ai_cleanup>, public threaded<uksf_ai_cleanup> {
public:
    uksf_ai_cleanup() {
        uksf_ai::getInstance()->preStart.connect([this]() {
            LOG(DEBUG) << "CLEANUP PRESTART";
            uksfCleanupKilled = client::host::registerFunction(
                "uksfCleanupKilled",
                "Adds object to killed list for cleanup",
                userFunctionWrapper<uksfCleanupKilledFunction>,
                types::GameDataType::NOTHING,
                types::GameDataType::OBJECT
            );
            uksfCleanupToggle = client::host::registerFunction(
                "uksfCleanupToggle",
                "Excludes/includes object in cleanup system",
                userFunctionWrapper<uksfCleanupToggleFunction>,
                types::GameDataType::NOTHING,
                types::GameDataType::ARRAY
            );
        });

        uksf_ai::getInstance()->preInit.connect([this]() {
            LOG(DEBUG) << "CLEANUP PREINIT";
            game_value enabledArgs({
                "uksf_ai_cleanup_enabled",
                "CHECKBOX",
                "Cleanup System",
                "UKSF",
                CLEANUP_ENABLED_DEFAULT,
                true
            });
            sqf::call(uksf_ai_common::CBA_Settings_fnc_init, enabledArgs);
            game_value delayArgs({
                "uksf_ai_cleanup_delay",
                "SLIDER",
                "Cleanup System Delay",
                "UKSF",
                { 30, 600, CLEANUP_DELAY_DEFAULT, 0 },
                true
            });
            sqf::call(uksf_ai_common::CBA_Settings_fnc_init, delayArgs);

            if (sqf::is_server()) {
                getInstance()->stopServerThread();
            }
        });

        uksf_ai::getInstance()->postInit.connect([this]() {
            LOG(DEBUG) << "CLEANUP POSTINIT";
            cleanupEnabled = (sqf::get_variable(sqf::mission_namespace(), "uksf_ai_cleanup_enabled", CLEANUP_ENABLED_DEFAULT));
            cleanupDelay = (sqf::get_variable(sqf::mission_namespace(), "uksf_ai_cleanup_distance", CLEANUP_DELAY_DEFAULT));

            if (!cleanupEnabled) {
                LOG(INFO) << "Cleanup system is disabled";
            } else {
                LOG(INFO) << "Cleanup system is enabled";
                if (sqf::is_server()) {
                    getInstance()->startServerThread();
                }
            };
        });

        uksf_ai::getInstance()->missionEnded.connect([this]() {
            LOG(DEBUG) << "CLEANUP MISSION ENDED";
            getInstance()->stopServerThread();
        });
    };

    ~uksf_ai_cleanup() {
        killedMap.clear();
    };

    virtual void serverFunction() {
        client::invoker_lock cleanupLock(true);
        if (serverThreadStop) return;
        cleanupLock.lock();
        for (auto entry = killedMap.begin(); entry != killedMap.end();) {
            auto killed = std::get<0>(entry->second);
            auto time = std::get<1>(entry->second);
            auto excluded = std::get<2>(entry->second);
            if ((!excluded && (time < (clock() / CLOCKS_PER_SEC)) && !sqf::alive(killed)) || killed.is_null()) {
                sqf::delete_vehicle(killed);
                entry = killedMap.erase(entry);
            } else {
                entry++;
            }
        }
        Sleep((DWORD)((cleanupDelay / 4) * CLOCKS_PER_SEC));
    };

    virtual void clientFunction() {};

private:
    std::unordered_map<size_t, killed_map_type> killedMap{};
    bool cleanupEnabled = CLEANUP_ENABLED_DEFAULT;
    float cleanupDelay = CLEANUP_DELAY_DEFAULT;

    types::registered_sqf_function uksfCleanupKilled;
    static game_value uksfCleanupKilledFunction(game_value param) {
        if (getInstance()->cleanupEnabled) {
            object killed = (object)param;
            auto &killedMap = getInstance()->killedMap;
            auto entry = killedMap.find(killed.hash());
            if (entry != killedMap.end()) {
                if (!std::get<2>(entry->second)) { // Not excluded
                    std::get<1>(entry->second) = (clock_t)((clock() / CLOCKS_PER_SEC) + (getInstance()->cleanupDelay * ((sqf::is_kind_of(std::get<0>(entry->second), "CAManBase")) ? 1 : 2))); // Update time killed
                }
            } else {
                killedMap.insert({ killed.hash(), killed_map_type({ killed, (clock_t)((clock() / CLOCKS_PER_SEC) + (getInstance()->cleanupDelay * ((sqf::is_kind_of(killed, "CAManBase")) ? 1 : 2))), false }) });
            }
        }
        return game_value();
    };

    types::registered_sqf_function uksfCleanupToggle;
    static game_value uksfCleanupToggleFunction(game_value params) {
        if (getInstance()->cleanupEnabled) {
            object killed = (object)params[0];
            bool forceExclude = (bool)params[1];
            auto &killedMap = getInstance()->killedMap;
            auto entry = killedMap.find(killed.hash());
            auto message = "Excluded from cleanup";
            if (entry != killedMap.end()) {
                if (!std::get<2>(entry->second) || forceExclude) { // Not excluded
                    std::get<2>(entry->second) = true; // Exclude
                } else {
                    std::get<2>(entry->second) = false; // Include    
                    message = "Included in cleanup";
                }
            } else { // Doesn't exist, add and exclude            
                killedMap.insert({ killed.hash(), killed_map_type({ killed, (clock_t)((clock() / CLOCKS_PER_SEC) + (getInstance()->cleanupDelay * ((sqf::is_kind_of(killed, "CAManBase")) ? 1 : 2))), true }) });
            }

            if (params.size() > 2) {
                object player = (object)params[2];
                sqf::remote_exec_call({ message }, "ace_common_fnc_displayTextStructured", player, false);
            }
        }
        return game_value();
    };
};
