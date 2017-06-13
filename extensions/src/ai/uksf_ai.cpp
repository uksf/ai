#include "uksf_ai.hpp"

void __cdecl intercept::pre_init() {
    LOG(DEBUG) << "MAIN PREINIT";
    uksf_common::getFunctions();
    uksf_ai::getInstance()->preInit();
}

void __cdecl intercept::post_init() {
    LOG(DEBUG) << "MAIN POSTINIT";
    uksf_ai::getInstance()->postInit();
}

void __cdecl intercept::on_frame() {
    uksf_ai::getInstance()->onFrame();
}

void __cdecl intercept::mission_stopped() {
    LOG(DEBUG) << "MAIN MISSION STOPPED";
    uksf_ai::getInstance()->missionStopped();
};

uksf_ai::uksf_ai() {
    new uksf_ai_caching();
}
