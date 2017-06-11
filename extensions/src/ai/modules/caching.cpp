#include "caching.hpp"

uksf_ai_caching::uksf_ai_caching() {
    uksf_ai::getInstance()->preInit.connect([this]() {
        LOG(DEBUG) << "CACHING SIGNAL PREINIT";
    });
    
    uksf_ai::getInstance()->postInit.connect([this]() {
        LOG(DEBUG) << "CACHING SIGNAL POSTINIT";
        if (intercept::sqf::has_interface()) {
            uksf_ai_caching::getInstance()->startClientThread();
        }
        if (intercept::sqf::is_server()) {
            uksf_ai_caching::getInstance()->startServerThread();
        }
    });

    uksf_ai::getInstance()->onFrame.connect([this]() {
        uksf_ai_caching::getInstance()->onFrameFunction();
    });

    uksf_ai::getInstance()->missionStopped.connect([this]() {
        LOG(DEBUG) << "CACHING SIGNAL MISSION STOPPED";
        if (intercept::sqf::has_interface()) {
            uksf_ai_caching::getInstance()->stopClientThread();
        }
        if (intercept::sqf::is_server()) {
            uksf_ai_caching::getInstance()->stopServerThread();
        }
    });
}

void uksf_ai_caching::startClientThread() {
    clientThreadStop = false;
    clientThread = std::thread(&uksf_ai_caching::clientThreadFunction, this);
}

void uksf_ai_caching::startServerThread() {
    serverThreadStop = false;
    serverThread = std::thread(&uksf_ai_caching::serverThreadFunction, this);
}

void uksf_ai_caching::stopClientThread() {
    clientThreadStop = true;
    clientThread.join();
}

void uksf_ai_caching::stopServerThread() {
    serverThreadStop = true;
    serverThread.join();
}

void uksf_ai_caching::clientThreadFunction() {
    while (!clientThreadStop) {
        {
            intercept::client::invoker_lock cachingClientLock;
            visibleUnits.clear();
            std::vector<intercept::types::group> groups = intercept::sqf::all_groups();
            auto group = groups.begin();
            while (group != groups.end()) {
                intercept::types::object leader = intercept::sqf::leader(*group);
                if (uksf_common::lineOfSight(leader, intercept::sqf::player(), true, true)) {
                    visibleUnits.push_back(leader);
                }
                ++group;
            }
        }
        Sleep(1000);
    }
}

void uksf_ai_caching::serverThreadFunction() {
    while (!serverThreadStop) {
        {
            intercept::client::invoker_lock cachingServerLock;
        }
        Sleep(5000);
    }
}

void uksf_ai_caching::onFrameFunction() {
    std::vector<intercept::types::object> units = intercept::sqf::all_units();
    auto unit = units.begin();
    while (unit != units.end()) {
        intercept::types::rv_color color{ 1.0f, 0.0f, 0.0f, 1.0f };
        if ((std::find(visibleUnits.begin(), visibleUnits.end(), *unit)) != visibleUnits.end()) {
            color = { 0.0f, 1.0f, 0.0f, 1.0f };
        }
        intercept::sqf::draw_icon_3d("\\a3\\ui_f_curator\\data\\logos\\arma3_curator_eye_32_ca.paa", color, intercept::sqf::get_pos_atl(*unit), 0.5f, 0.5f, 0, "", 0, 0, "TahomaB", "center", true);
        ++unit;
    }
}
