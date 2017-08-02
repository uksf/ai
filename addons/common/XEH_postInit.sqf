#include "script_component.hpp"

["unit", {
    uksfCommonSetPlayer ACE_player;
}, true] call CBA_fnc_addPlayerEventHandler;

["ACE_controlledUAV", {
    params ["", "_seatAI"];
    if (!isNull _seatAI) then {
        uksfCommonSetPlayer _seatAI;
    };
}] call CBA_fnc_addEventHandler;
