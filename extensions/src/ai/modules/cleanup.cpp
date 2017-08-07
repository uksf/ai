#include "cleanup.hpp"


uksf_ai_cleanup::uksf_ai_cleanup() {
    uksf_ai::getInstance().preStart.connect([this]() {
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

    uksf_ai::getInstance().preInit.connect([this]() {
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
            getInstance().stopServerThread();
        }
    });

    uksf_ai::getInstance().postInit.connect([this]() {
        LOG(DEBUG) << "CLEANUP POSTINIT";
        _cleanupEnabled = (sqf::get_variable(sqf::mission_namespace(), "uksf_ai_cleanup_enabled", CLEANUP_ENABLED_DEFAULT));
        _cleanupDelay = (sqf::get_variable(sqf::mission_namespace(), "uksf_ai_cleanup_distance", CLEANUP_DELAY_DEFAULT));

        if (!_cleanupEnabled) {
            LOG(INFO) << "Cleanup system is disabled";
        } else {
            LOG(INFO) << "Cleanup system is enabled";
            if (sqf::is_server()) {
                getInstance().startServerThread();
            }
        };
    });

    uksf_ai::getInstance().missionEnded.connect([this]() {
        LOG(DEBUG) << "CLEANUP MISSION ENDED";
        getInstance().stopServerThread();
    });
};

uksf_ai_cleanup::~uksf_ai_cleanup() {
    _killedMap.clear();
};

void uksf_ai_cleanup::serverFunction() {
    {
        client::invoker_lock cleanupLock;
        for (auto entry = _killedMap.begin(); entry != _killedMap.end();) {
            auto killed = std::get<0>(entry->second);
            auto time = std::get<1>(entry->second);
            auto excluded = std::get<2>(entry->second);
            if ((!excluded && (time < (clock() / CLOCKS_PER_SEC)) && !sqf::alive(killed)) || killed.is_null()) {
                sqf::delete_vehicle(killed);
                entry = _killedMap.erase(entry);
            } else {
                entry++;
            }
        }
    }
    Sleep((DWORD)((_cleanupDelay / 4) * CLOCKS_PER_SEC));
};

game_value uksf_ai_cleanup::uksfCleanupKilledFunction(game_value param) {
    if (getInstance()._cleanupEnabled) {
        object killed = (object)param;
        auto &_killedMap = getInstance()._killedMap;
        auto entry = _killedMap.find(killed.hash());
        if (entry != _killedMap.end()) {
            if (!std::get<2>(entry->second)) { // Not excluded
                std::get<1>(entry->second) = (clock_t)((clock() / CLOCKS_PER_SEC) + (getInstance()._cleanupDelay * ((sqf::is_kind_of(std::get<0>(entry->second), "CAManBase")) ? 1 : 2))); // Update time killed
            }
        } else {
            _killedMap.insert({ killed.hash(), killed_map_type({ killed, (clock_t)((clock() / CLOCKS_PER_SEC) + (getInstance()._cleanupDelay * ((sqf::is_kind_of(killed, "CAManBase")) ? 1 : 2))), false }) });
        }
    }
    return game_value();
};

game_value uksf_ai_cleanup::uksfCleanupToggleFunction(game_value params) {
    if (getInstance()._cleanupEnabled) {
        object killed = (object)params[0];
        bool forceExclude = (bool)params[1];
        auto &_killedMap = getInstance()._killedMap;
        auto entry = _killedMap.find(killed.hash());
        auto message = "Excluded from cleanup";
        if (entry != _killedMap.end()) {
            if (!std::get<2>(entry->second) || forceExclude) { // Not excluded
                std::get<2>(entry->second) = true; // Exclude
            } else {
                std::get<2>(entry->second) = false; // Include    
                message = "Included in cleanup";
            }
        } else { // Doesn't exist, add and exclude            
            _killedMap.insert({ killed.hash(), killed_map_type({ killed, (clock_t)((clock() / CLOCKS_PER_SEC) + (getInstance()._cleanupDelay * ((sqf::is_kind_of(killed, "CAManBase")) ? 1 : 2))), true }) });
        }

        if (params.size() > 2) {
            object player = (object)params[2];
            sqf::remote_exec_call({ message }, "ace_common_fnc_displayTextStructured", player, false);
        }
    }
    return game_value();
};
