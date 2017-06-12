#include "caching.hpp"

uksf_ai_caching::uksf_ai_caching() {
    uksf_ai::getInstance()->preInit.connect([this]() {
        LOG(DEBUG) << "CACHING SIGNAL PREINIT";
    });
    
    uksf_ai::getInstance()->postInit.connect([this]() {
        LOG(DEBUG) << "CACHING SIGNAL POSTINIT";
        if (sqf::has_interface()) {
            uksf_ai_caching::getInstance()->startClientThread();
        }
        if (sqf::is_server()) {
            uksf_ai_caching::getInstance()->startServerThread();
        }
    });

    uksf_ai::getInstance()->onFrame.connect([this]() {
        uksf_ai_caching::getInstance()->onFrameFunction();
    });

    uksf_ai::getInstance()->missionStopped.connect([this]() {
        LOG(DEBUG) << "CACHING SIGNAL MISSION STOPPED";
        if (sqf::has_interface()) {
            uksf_ai_caching::getInstance()->stopClientThread();
        }
        if (sqf::is_server()) {
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
            client::invoker_lock cachingClientLock;
            visibleUnits.clear();
            std::vector<types::group> groups = sqf::all_groups();
            for (auto& group : groups) {
                types::object leader = sqf::leader(group);
                if (uksf_common::lineOfSight(leader, sqf::player(), true, true)) {
                    visibleUnits.push_back(leader);
                }
            }
        }
        Sleep(1000);
    }
}

void uksf_ai_caching::serverThreadFunction() {
    while (!serverThreadStop) {
        {
            client::invoker_lock cachingServerLock;
        }
        Sleep(5000);
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
