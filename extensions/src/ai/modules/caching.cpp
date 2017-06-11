#include "caching.hpp"

uksf_ai_caching::uksf_ai_caching() {
    instance = this;

    uksf_ai::getInstance()->preInit.connect([this]() {
        LOG(DEBUG) << "CACHING SIGNAL PREINIT";
    });
    
    uksf_ai::getInstance()->postInit.connect([this]() {
        LOG(DEBUG) << "CACHING SIGNAL POSTINIT";
        uksf_ai_caching::getInstance()->startClientThread();
    });

    uksf_ai::getInstance()->onFrame.connect([this]() {
        uksf_ai_caching::getInstance()->onFrameFunction();
    });

    uksf_ai::getInstance()->missionStopped.connect([this]() {
        LOG(DEBUG) << "CACHING SIGNAL MISSION STOPPED";
        uksf_ai_caching::getInstance()->stopClientThread();
    });
}

uksf_ai_caching* uksf_ai_caching::instance = 0;

uksf_ai_caching* uksf_ai_caching::getInstance() {
    if (instance == 0) {
        instance = new uksf_ai_caching();
    }
    return instance;
}

void uksf_ai_caching::startClientThread() {
    threadStop = false;
    clientThread = std::thread(&uksf_ai_caching::clientThreadFunction, this);
}

void uksf_ai_caching::stopClientThread() {
    threadStop = true;
    clientThread.join();
}

void uksf_ai_caching::clientThreadFunction() {
    LOG(DEBUG) << "START TEST THREAD";
    while (!threadStop) {
        {
            intercept::client::invoker_lock losLock;
            visibleUnits.clear();
            std::vector<intercept::types::group> groups = intercept::sqf::all_groups();
            auto group = groups.begin();
            while (group != groups.end()) {
                intercept::types::object leader = intercept::sqf::leader(*group);
                if (uksf_shared::lineOfSight(leader, intercept::sqf::player(), true, true)) {
                    visibleUnits.push_back(leader);
                }
                ++group;
            }
        }
        Sleep(1000);
    }
    LOG(DEBUG) << "SHUT DOWN TEST THREAD";
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
