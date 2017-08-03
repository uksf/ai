#pragma once
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <cstdint>
#include <atomic>
#include <stdlib.h>
#include <algorithm>
#include <variant>
#include <vector>
#include <functional>
#include <chrono>

#include "intercept.hpp"
using namespace intercept;

#include "logging.hpp"
#include "signalslot.hpp"
#include "singleton.hpp"
#include "threaded.hpp"

class uksf_ai : public singleton<uksf_ai> {
public:
    uksf_ai();

    Signal<void()> preStart;
    Signal<void()> preInit;
    Signal<void()> postInit;
    Signal<void()> onFrame;
    Signal<void()> missionEnded;
};
