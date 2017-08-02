#pragma once
#include "uksf_ai.hpp"

#include "caching.hpp"
#include "cleanup.hpp"

class uksf_ai_common : public singleton<uksf_ai_common> {
public:
    uksf_ai_common();

    types::registered_sqf_function uksfCommonSetPlayer;
    static game_value uksfCommonSetPlayerFunction(game_value param);

    static game_value CBA_Settings_fnc_init;
    static bool thread_run;
    static object player;

    static float getZoom();
    static bool lineOfSight(object& target, object& source, bool zoomCheck, bool groupCheck);
};
