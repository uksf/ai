#pragma once
#include "uksf_ai.hpp"

class uksf_ai_common : public singleton<uksf_ai_common> {
public:
    uksf_ai_common();

    types::registered_sqf_function uksfCommonSetPlayer;
    static game_value uksfCommonSetPlayerFunction(game_value param);

    randomgen *rand;
    static game_value CBA_Settings_fnc_init;
    static bool thread_run;
    static object player;

    static float getZoom();
    static bool lineOfSight(object& target, object& source, bool zoomCheck, bool groupCheck);
    static side getSide(int sideNumber);
};
