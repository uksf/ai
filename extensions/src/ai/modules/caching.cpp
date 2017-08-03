#pragma once
#include "common.hpp"

#define CACHING_ENABLED_DEFAULT true
#define CACHING_DISTANCE_DEFAULT 750
#define CACHING_TIME_SERVER 10
#define CACHING_TIME_CLIENT 5

typedef std::tuple<group, clock_t> cache_map_type;

class uksf_ai_caching : public singleton<uksf_ai_caching>, public threaded<uksf_ai_caching> {
public:
    uksf_ai_caching() {
        uksf_ai::getInstance()->preStart.connect([this]() {
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

        uksf_ai::getInstance()->preInit.connect([this]() {
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
                getInstance()->stopServerThread();
            }
            if (sqf::has_interface()) {
                getInstance()->stopClientThread();
            }
            getInstance()->cacheMap.clear();
        });

        uksf_ai::getInstance()->postInit.connect([this]() {
            LOG(DEBUG) << "CACHING POSTINIT";
            cachingEnabled = (sqf::get_variable(sqf::mission_namespace(), "uksf_ai_caching_enabled", CACHING_ENABLED_DEFAULT));
            cachingDistance = (sqf::get_variable(sqf::mission_namespace(), "uksf_ai_caching_distance", CACHING_DISTANCE_DEFAULT));

            if (!cachingEnabled) {
                LOG(INFO) << "Caching system is disabled";
                sqf::enable_dynamic_simulation_system(false);
            } else {
                LOG(INFO) << "Caching system is enabled";
                sqf::enable_dynamic_simulation_system(true);
                sqf::set_dynamic_simulation_distance("Group", cachingDistance);
                sqf::set_dynamic_simulation_distance("Vehicle", cachingDistance);
                sqf::set_dynamic_simulation_distance("EmptyVehicle", cachingDistance / 3);
                sqf::set_dynamic_simulation_distance("Prop", cachingDistance / 10);
                sqf::set_dynamic_simulation_distance_coef("IsMoving", 1.5f);

                if (sqf::is_server()) {
                    getInstance()->startServerThread();
                }
                if (sqf::has_interface()) {
                    getInstance()->startClientThread();
                }
            };
        });

        uksf_ai::getInstance()->missionEnded.connect([this]() {
            LOG(DEBUG) << "CACHING MISSION ENDED";
            getInstance()->cacheMap.clear();
        });
    };

    ~uksf_ai_caching() {
        cacheMap.clear();
    };

    virtual void serverFunction() {
        client::invoker_lock cachingLock(true);
        if (serverThreadStop) return;
        cachingLock.lock();

        for (auto& entry : cacheMap) {
            auto group = std::get<0>(entry.second);
            auto leader = sqf::leader(group);
            auto time = std::get<1>(entry.second);
            if (((time + CACHING_TIME_SERVER) < (clock() / CLOCKS_PER_SEC)) && !sqf::dynamic_simulation_enabled(group)) {
                sqf::remote_exec_call({ { group, false, false } }, "uksfCachingUpdate", 0, false);
            }
        }
        Sleep(5000);
    };

    virtual void clientFunction() {
        client::invoker_lock cachingLock(true);
        if (clientThreadStop) return;
        cachingLock.lock();

        for (auto& entry : cacheMap) {
            auto group = std::get<0>(entry.second);
            auto leader = sqf::leader(group);
            auto time = std::get<1>(entry.second);
            if ((((time + CACHING_TIME_CLIENT) < (clock() / CLOCKS_PER_SEC)) || !sqf::simulation_enabled(leader))) {
                auto distance = (sqf::get_pos_world(leader)).distance(sqf::get_pos_world(uksf_ai_common::player));
                if (distance > cachingDistance && (distance < sqf::get_object_view_distance().object_distance) && uksf_ai_common::lineOfSight(leader, uksf_ai_common::player, true, true)) {
                    if (sqf::dynamic_simulation_enabled(group)) {
                        sqf::remote_exec_call({ { group, true, false } }, "uksfCachingUpdate", 0, false);
                    }
                    std::get<1>(entry.second) = (clock_t)(clock() / CLOCKS_PER_SEC);
                }
            }
        }
        Sleep(1000);
    };

private:
    std::unordered_map<size_t, cache_map_type> cacheMap;
    bool cachingEnabled = CACHING_ENABLED_DEFAULT;
    float cachingDistance = CACHING_DISTANCE_DEFAULT;

    types::registered_sqf_function uksfCachingAdd;
    static game_value uksfCachingAddFunction(game_value param) { // Should be executed globally
        if (getInstance()->cachingEnabled) {
            object unit = (object)param;
            if (sqf::is_player(unit) || sqf::is_kind_of(sqf::vehicle(unit), "Air")) return game_value();
            auto group = sqf::get_group(unit);
            auto &cacheMap = getInstance()->cacheMap;
            auto entry = cacheMap.find(group.hash());
            if (entry == cacheMap.end()) { // Does not exist in map, include (add)
                cacheMap.insert({ group.hash(), cache_map_type({ group, (clock_t)(0) }) });
            }
        }
        return game_value();
    };

    types::registered_sqf_function uksfCachingRemove;
    static game_value uksfCachingRemoveFunction(game_value param) { // Should be executed globally
        if (getInstance()->cachingEnabled) {
            object cached = (object)param;
            auto group = sqf::get_group(cached);
            auto &cacheMap = getInstance()->cacheMap;
            auto entry = cacheMap.find(group.hash());
            if (entry != cacheMap.end()) { // Exists in map, exclude (remove)
                uksfCachingUpdateFunction({ group, true, true });
                return true;
            }
        }
        return false;
    };

    types::registered_sqf_function uksfCachingUpdate;
    static game_value uksfCachingUpdateFunction(game_value params) { // Should be executed globally
        group update = (group)params[0];
        bool enable = (bool)params[1];
        bool updateMap = (bool)params[2];
        auto &cacheMap = getInstance()->cacheMap;
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
        for (auto unit : units) {
            sqf::enable_simulation(unit, enable);
            auto group = sqf::get_group(unit);
            groups.insert({ group.hash(), group });
        }

        for (auto& entry : groups) {
            auto group = entry.second;
            sqf::enable_dynamic_simulation(group, !enable);
            if (updateMap) {
                auto entry = cacheMap.find(group.hash());
                if (entry != cacheMap.end()) { // Exists in map, exclude (remove)
                    cacheMap.erase(entry);
                } else { // Does not exist in map, include (add)
                    cacheMap.insert({ group.hash(), cache_map_type({ group, (clock_t)(0) }) });
                }
            }
        }
        return game_value();
    };

    types::registered_sqf_function uksfCachingToggle;
    static game_value uksfCachingToggleFunction(game_value params) { // Should be executed globally
        if (getInstance()->cachingEnabled) {
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
}; 
