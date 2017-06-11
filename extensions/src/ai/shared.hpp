#pragma once
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <cstdint>
#include <atomic>
#include <stdlib.h>
#include <algorithm>
#include <variant>

#include "intercept.hpp"
#include "logging.hpp"
#include "signalslot.hpp"

//TODO: Remove after exposed in intercept
constexpr std::string_view operator ""_sv(char const* str, std::size_t len) noexcept {
    return { str, len };
};

class uksf_shared {
public:
    static float getZoom();
    static bool lineOfSight(intercept::types::object& target, intercept::types::object& source, bool zoomCheck, bool groupCheck);
};
