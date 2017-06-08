#pragma once
#include <windows.h>
#include <stdio.h>
#include <cstdint>
#include <atomic>

#include "intercept.hpp"
#include "logging.hpp"

#include "SignalSlot.hpp"

//TODO: Remove after exposed in intercept
constexpr std::string_view operator ""_sv(char const* str, std::size_t len) noexcept {
    return { str, len };
};

class uksf_ai {
public:
    uksf_ai();

    static uksf_ai& getInstance();

    Signal<void()> preInit;
    Signal<void()> postInit;
};
