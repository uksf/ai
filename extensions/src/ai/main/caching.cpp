#include <windows.h>
#include <stdio.h>
#include <cstdint>
#include <atomic>
#include "uksf_ai.hpp"

#include "caching.hpp"

uksf_ai_caching::uksf_ai_caching() {
    LOG(INFO) << "Caching call";

    uksf_ai::getInstance().preInit.connect([this]() {
        intercept::sqf::diag_log("CACHING SIGNAL PREINIT"_sv);
    });

    uksf_ai::getInstance().postInit.connect([this]() {
        intercept::sqf::diag_log("CACHING SIGNAL POSTINIT"_sv);
    });
}
