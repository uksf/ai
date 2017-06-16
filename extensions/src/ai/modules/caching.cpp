#include "caching.hpp"

uksf_ai_caching::uksf_ai_caching() {
    uksf_ai::getInstance()->preInit.connect([this]() {
        LOG(DEBUG) << "CACHING SIGNAL PREINIT";
        LOG(DEBUG) << uksf_common::CBA_Settings_fnc_init.data;
        game_value enabled_args({
            "uksf_ai_caching_enabled",
            "CHECKBOX",
            "Caching System",
            "UKSF",
            true,
            true
        });
        sqf::call(uksf_common::CBA_Settings_fnc_init, enabled_args);
        game_value distance_args({
            "uksf_ai_caching_distance",
            "SLIDER",
            "Caching System Distance",
            "UKSF",
            { 500, 2000, 750, 0 },
            true
        });
        sqf::call(uksf_common::CBA_Settings_fnc_init, distance_args);
    });
        
    uksf_ai::getInstance()->postInit.connect([this]() {
        LOG(DEBUG) << "CACHING SIGNAL POSTINIT";
        uksf_ai_caching::getInstance()->setCachingEnabled(sqf::get_variable(sqf::mission_namespace(), "uksf_ai_caching_enabled", true));
        uksf_ai_caching::getInstance()->setCachingDistance(sqf::get_variable(sqf::mission_namespace(), "uksf_ai_caching_distance", 750));

        if (!uksf_ai_caching::getInstance()->getCachingEnabled()) {
            LOG(INFO) << "Caching system is disabled";
            sqf::enable_dynamic_simulation_system(false);
        } else {
            LOG(INFO) << "Caching system is enabled";
            sqf::enable_dynamic_simulation_system(true);
            sqf::set_dynamic_simulation_distance("Group", uksf_ai_caching::getInstance()->getCachingDistance());
            sqf::set_dynamic_simulation_distance("Vehicle", uksf_ai_caching::getInstance()->getCachingDistance());
            sqf::set_dynamic_simulation_distance("EmptyVehicle", 250);
            sqf::set_dynamic_simulation_distance("Prop", 50);
            sqf::set_dynamic_simulation_distance_coef("IsMoving", 1.5f);
        };

        if (sqf::is_server()) {
            uksf_ai_caching::getInstance()->startServerThread();
        }
        if (sqf::has_interface()) {
            uksf_ai_caching::getInstance()->startClientThread();
        }
    });

    uksf_ai::getInstance()->onFrame.connect([this]() {
        //uksf_ai_caching::getInstance()->onFrameFunction();
    });

    uksf_ai::getInstance()->missionStopped.connect([this]() {
        LOG(DEBUG) << "CACHING SIGNAL MISSION STOPPED";
        if (sqf::is_server()) {
            uksf_ai_caching::getInstance()->stopServerThread();
        }
        if (sqf::has_interface()) {
            uksf_ai_caching::getInstance()->stopClientThread();
        }
    });
}

void uksf_ai_caching::startServerThread() {
    serverThreadStop = false;
    serverThread = std::thread(&uksf_ai_caching::serverThreadFunction, this);
}

void uksf_ai_caching::startClientThread() {
    clientThreadStop = false;
    clientThread = std::thread(&uksf_ai_caching::clientThreadFunction, this);
}

void uksf_ai_caching::stopServerThread() {
    serverThreadStop = true;
    serverThread.join();
}

void uksf_ai_caching::stopClientThread() {
    clientThreadStop = true;
    clientThread.join();
}

void uksf_ai_caching::serverThreadFunction() {
    while (!serverThreadStop) {
        {
            client::invoker_lock cachingLock;
            std::vector<group> groups = sqf::all_groups();
            /*for (auto& group : groups) {
                object leader = sqf::leader(group);
                if (!(sqf::get_variable(group, "uksf_ai_caching_excluded", false)) && !sqf::dynamic_simulation_enabled(group) && !sqf::is_player(leader) &&
                    sqf::is_kind_of(sqf::vehicle(leader), "Air") && (((float)sqf::get_variable(group, "uksf_ai_caching_time", 0) + 15) < sqf::diag_ticktime())) {
                    sqf::remote_exec_call({group, true}, "enableDynamicSimulation", 0, false);
                }
            }*/
        }
        Sleep(5000);
    }
}

void uksf_ai_caching::clientThreadFunction() {
    while (!clientThreadStop) {
        {
            client::invoker_lock cachingLock;
            object player = sqf::is_null(sqf::get_connected_uav(player)) ? sqf::gunner(sqf::get_connected_uav(sqf::player())) : sqf::get_variable(sqf::mission_namespace(), "bis_fnc_moduleRemoteControl_unit", sqf::player());
            std::vector<group> groups = sqf::all_groups();
            /*for (auto& group : groups) {
                object leader = sqf::leader(group);
                if (!(sqf::get_variable(group, "uksf_ai_caching_excluded", false)) && !sqf::is_player(leader) && sqf::is_kind_of(sqf::vehicle(leader), "Air") &&
                    (sqf::get_pos_world(leader)).distance(sqf::get_pos_world(player)) > getCachingDistance() && (sqf::get_pos_world(leader)).distance(sqf::get_pos_world(player)) < sqf::get_object_view_distance().object_distance &&
                    ((((float)sqf::get_variable(group, "uksf_ai_caching_time", 0) + 10) < sqf::diag_ticktime()) || !sqf::simulation_enabled(leader)) && uksf_common::lineOfSight(leader, player, true, true)) {
                    if (sqf::dynamic_simulation_enabled(group)) {
                        sqf::remote_exec_call({ group, false }, "enableDynamicSimulation", 0, false);
                    }
                    sqf::set_variable(group, "uksf_ai_caching_time", sqf::diag_ticktime());
                }
            }*/
        }
        Sleep(1000);
    }
}

void uksf_ai_caching::onFrameFunction() {
    std::vector<types::object> units = sqf::all_units();
    for (auto& unit : units) {
        types::rv_color color{ 1.0f, 0.0f, 0.0f, 1.0f };
        if ((std::find(visibleUnits.begin(), visibleUnits.end(), unit)) != visibleUnits.end()) {
            color = { 0.0f, 1.0f, 0.0f, 1.0f };
        }
        sqf::draw_icon_3d("\\a3\\ui_f_curator\\data\\logos\\arma3_curator_eye_32_ca.paa", color, sqf::get_pos_atl(unit), 0.5f, 0.5f, 0, "", 0, 0, "TahomaB", "center", true);
    }
}

const bool& uksf_ai_caching::getCachingEnabled() {
    return cachingEnabled;
}
void uksf_ai_caching::setCachingEnabled(const bool& value) {
    cachingEnabled = value;
}

const float& uksf_ai_caching::getCachingDistance() {
    return cachingDistance;
}
void uksf_ai_caching::setCachingDistance(const float& value) {
    cachingDistance = value;
}
