#include "caching.hpp"

uksf_ai_caching::uksf_ai_caching() {
    uksf_ai::getInstance().preStart.connect([this]() {
        LOG(DEBUG) << "CACHING PRESTART";
        uksfCachingAdd = client::host::registerFunction(
            "uksfCachingAdd",
            "Adds group to caching system",
            userFunctionWrapper<uksfCachingAddFunction>,
            types::GameDataType::NOTHING,
            types::GameDataType::OBJECT
        );
        uksfCachingRemove = client::host::registerFunction(
            "uksfCachingRemove",
            "Removes group from caching system",
            userFunctionWrapper<uksfCachingRemoveFunction>,
            types::GameDataType::BOOL,
            types::GameDataType::OBJECT
        );
        uksfCachingUpdate = client::host::registerFunction(
            "uksfCachingUpdate",
            "Updates cached state. Includes vehicle occupants",
            userFunctionWrapper<uksfCachingUpdateFunction>,
            types::GameDataType::NOTHING,
            types::GameDataType::ARRAY
        );
        uksfCachingToggle = client::host::registerFunction(
            "uksfCachingToggle",
            "Excludes/includes object in caching system",
            userFunctionWrapper<uksfCachingToggleFunction>,
            types::GameDataType::NOTHING,
            types::GameDataType::ARRAY
        );
    });

    uksf_ai::getInstance().preInit.connect([this]() {
        LOG(DEBUG) << "CACHING PREINIT";
        game_value enabledArgs({
            "uksf_ai_caching_enabled",
            "CHECKBOX",
            "Caching System",
            "UKSF",
            CACHING_ENABLED_DEFAULT,
            true
        });
        sqf::call(uksf_ai_common::CBA_Settings_fnc_init, enabledArgs);
        game_value distanceArgs({
            "uksf_ai_caching_distance",
            "SLIDER",
            "Caching System Distance",
            "UKSF",
            { 500, 2000, CACHING_DISTANCE_DEFAULT, 0 },
            true
        });
        sqf::call(uksf_ai_common::CBA_Settings_fnc_init, distanceArgs);

        if (sqf::is_server()) {
            getInstance().stopServerThread();
        }
        if (sqf::has_interface()) {
            getInstance().stopClientThread();
        }
        getInstance()._cacheMap.clear();
    });

    uksf_ai::getInstance().postInit.connect([this]() {
        LOG(DEBUG) << "CACHING POSTINIT";
        _cachingEnabled = (sqf::get_variable(sqf::mission_namespace(), "uksf_ai_caching_enabled", CACHING_ENABLED_DEFAULT));
        _cachingDistance = (sqf::get_variable(sqf::mission_namespace(), "uksf_ai_caching_distance", CACHING_DISTANCE_DEFAULT));

        if (!_cachingEnabled) {
            LOG(INFO) << "Caching system is disabled";
            sqf::enable_dynamic_simulation_system(false);
        } else {
            LOG(INFO) << "Caching system is enabled";
            sqf::enable_dynamic_simulation_system(true);
            sqf::set_dynamic_simulation_distance("Group", _cachingDistance);
            sqf::set_dynamic_simulation_distance("Vehicle", _cachingDistance);
            sqf::set_dynamic_simulation_distance("EmptyVehicle", _cachingDistance / 3);
            sqf::set_dynamic_simulation_distance("Prop", _cachingDistance / 10);
            sqf::set_dynamic_simulation_distance_coef("IsMoving", 1.5f);

            if (sqf::is_server()) {
                getInstance().startServerThread();
            }
            if (sqf::has_interface()) {
                getInstance().startClientThread();
            }
        };
    });

    uksf_ai::getInstance().missionEnded.connect([this]() {
        LOG(DEBUG) << "CACHING MISSION ENDED";
        getInstance()._cacheMap.clear();
    });
};

uksf_ai_caching::~uksf_ai_caching() {
    _cacheMap.clear();
};

void uksf_ai_caching::serverFunction() {
    {
        client::invoker_lock cachingLock;
        for (auto& entry : _cacheMap) {
            auto group = std::get<0>(entry.second);
            auto leader = sqf::leader(group);
            auto time = std::get<1>(entry.second);
            if (((time + CACHING_TIME_SERVER) < (clock() / CLOCKS_PER_SEC)) && !sqf::dynamic_simulation_enabled(group)) {
                sqf::remote_exec_call({ { group, false, false } }, "uksfCachingUpdate", 0, false);
            }
        }
    }
    Sleep(5000);
};

void uksf_ai_caching::clientFunction() {
    {
        client::invoker_lock cachingLock;
        for (auto& entry : _cacheMap) {
            auto group = std::get<0>(entry.second);
            auto leader = sqf::leader(group);
            auto time = std::get<1>(entry.second);
            if ((((time + CACHING_TIME_CLIENT) < (clock() / CLOCKS_PER_SEC)) || !sqf::simulation_enabled(leader))) {
                auto distance = (sqf::get_pos_world(leader)).distance(sqf::get_pos_world(uksf_ai_common::player));
                if (distance > _cachingDistance && (distance < sqf::get_object_view_distance().object_distance) && uksf_ai_common::lineOfSight(leader, uksf_ai_common::player, true, true)) {
                    if (sqf::dynamic_simulation_enabled(group)) {
                        sqf::remote_exec_call({ { group, true, false } }, "uksfCachingUpdate", 0, false);
                    }
                    std::get<1>(entry.second) = (clock_t)(clock() / CLOCKS_PER_SEC);
                }
            }
        }
    }
    Sleep(1000);
};

game_value uksf_ai_caching::uksfCachingAddFunction(game_value param) { // Should be executed globally
    if (getInstance()._cachingEnabled) {
        object unit = (object)param;
        if (sqf::is_player(unit) || sqf::is_kind_of(sqf::vehicle(unit), "Air")) return game_value();
        auto group = sqf::get_group(unit);
        auto &_cacheMap = getInstance()._cacheMap;
        auto entry = _cacheMap.find(group.hash());
        if (entry == _cacheMap.end()) { // Does not exist in map, include (add)
            _cacheMap.insert({ group.hash(), cache_map_type({ group, (clock_t)(0) }) });
        }
    }
    return game_value();
};

game_value uksf_ai_caching::uksfCachingRemoveFunction(game_value param) { // Should be executed globally
    if (getInstance()._cachingEnabled) {
        object cached = (object)param;
        auto group = sqf::get_group(cached);
        auto &_cacheMap = getInstance()._cacheMap;
        auto entry = _cacheMap.find(group.hash());
        if (entry != _cacheMap.end()) { // Exists in map, exclude (remove)
            uksfCachingUpdateFunction({ group, true, true });
            return true;
        }
    }
    return false;
};

game_value uksf_ai_caching::uksfCachingUpdateFunction(game_value params) { // Should be executed globally
    group update = (group)params[0];
    bool enable = (bool)params[1];
    bool updateMap = (bool)params[2];
    auto &_cacheMap = getInstance()._cacheMap;
    auto units = sqf::units(update);
    std::vector<object> crewUnits = {};
    auto vehicle = sqf::object_parent(sqf::leader(update));
    if (!vehicle.is_null()) {
        crewUnits = sqf::crew(vehicle);
    }
    units.insert(std::end(units), std::begin(crewUnits), std::end(crewUnits));
    std::sort(units.begin(), units.end());
    units.erase(std::unique(units.begin(), units.end()), units.end());
    std::unordered_map<size_t, group> groups{};
    for (auto& unit : units) {
        sqf::enable_simulation(unit, enable);
        auto group = sqf::get_group(unit);
        groups.insert({ group.hash(), group });
    }

    for (auto& entry : groups) {
        auto group = entry.second;
        sqf::enable_dynamic_simulation(group, !enable);
        if (updateMap) {
            auto entry = _cacheMap.find(group.hash());
            if (entry != _cacheMap.end()) { // Exists in map, exclude (remove)
                _cacheMap.erase(entry);
            } else { // Does not exist in map, include (add)
                _cacheMap.insert({ group.hash(), cache_map_type({ group, (clock_t)(0) }) });
            }
        }
    }
    return game_value();
};

game_value uksf_ai_caching::uksfCachingToggleFunction(game_value params) { // Should be executed globally
    if (getInstance()._cachingEnabled) {
        object unit = (object)params[0];
        auto message = "Excluded from caching";
        if (!((bool)uksfCachingRemoveFunction(unit))) { // Does not exist in map, include (add)
            uksfCachingAddFunction(unit);
            message = "Included in caching";
        }

        if (params.size() > 1) {
            object player = (object)params[1];
            sqf::remote_exec_call({ message }, "ace_common_fnc_displayTextStructured", player, false);
        }
    }
    return game_value();
};
