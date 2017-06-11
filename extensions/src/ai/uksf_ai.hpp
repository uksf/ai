#pragma once
#include "shared.hpp"
#include "caching.hpp"

class uksf_ai {
public:
    void start();

    static uksf_ai* getInstance();

    Signal<void()> preInit;
    Signal<void()> postInit;
    Signal<void()> onFrame;
    Signal<void()> missionStopped;

private:
    static uksf_ai* instance;
};
