#include "uksf_ai.hpp"
#include "common.hpp"

#include "caching.hpp"
#include "cleanup.hpp"
#include "spawning.hpp"

void __cdecl intercept::pre_start() {
    LOG(DEBUG) << "MAIN PRESTART";
    uksf_ai::getInstance().preStart();
}

void __cdecl intercept::pre_init() {
    LOG(DEBUG) << "MAIN PREINIT";
    uksf_ai::getInstance().preInit();
}

void __cdecl intercept::post_init() {
    LOG(DEBUG) << "MAIN POSTINIT";
    uksf_ai::getInstance().postInit();
}

void __cdecl intercept::on_frame() {
    uksf_ai::getInstance().onFrame();
}

void __cdecl intercept::mission_end() {
    LOG(DEBUG) << "MAIN MISSION ENDED";
    uksf_ai::getInstance().missionEnded();
}

uksf_ai::uksf_ai() {
    uksf_ai_common::getInstance();
    uksf_ai_caching::getInstance();
    uksf_ai_cleanup::getInstance();
    uksf_ai_spawning::getInstance();
}
