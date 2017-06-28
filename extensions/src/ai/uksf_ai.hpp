#pragma once
#include "common.hpp"
#include "caching.hpp"

class uksf_ai: public singleton<uksf_ai> {
public:
    uksf_ai();

    Signal<void()> preInit;
    Signal<void()> postInit;
    Signal<void()> onFrame;
    Signal<void()> missionEnded;
};
