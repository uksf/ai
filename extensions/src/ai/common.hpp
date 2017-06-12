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

#include "intercept.hpp"
using namespace intercept;

#include "logging.hpp"
#include "signalslot.hpp"
#include "singleton.hpp"

#define COMPONENT common
#include "macros.hpp"

//TODO: Remove after exposed in intercept
constexpr std::string_view operator ""_sv(char const* str, std::size_t len) noexcept {
    return { str, len };
};

class uksf_common {
public:
    static float getZoom();
    static bool lineOfSight(types::object& target, types::object& source, bool zoomCheck, bool groupCheck);
};
