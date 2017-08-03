#include "script_component.hpp"

["unit", {
    uksfCommonSetPlayer (_this select 0);
    [[(_this select 0)]] remoteExecCall ["uksfCachingToggle", 0];
    if (!(isNull CBA_events_oldUnit)) then {
        [[(_this select 1)]] remoteExecCall ["uksfCachingToggle", 0];
    };
}, true] call CBA_fnc_addPlayerEventHandler;

["ACE_controlledUAV", {
    params ["", "_seatAI"];
    if (!(isNull _seatAI)) then {
        uksfCommonSetPlayer _seatAI;
    } else {
        uksfCommonSetPlayer ACE_player;
    };
}] call CBA_fnc_addEventHandler;

[{
    params ["", "_idPFH"];
    if (!(isNull ACE_player)) exitWith {
        [_idPFH] call CBA_fnc_removePerFrameHandler;
        uksfCommonSetPlayer ACE_player;
    };
}, 0, []] call CBA_fnc_addPerFrameHandler;
