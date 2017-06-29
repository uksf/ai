/*
    Author:
        Tim Beswick

    Description:
        Toggles unit cleanup state

    Parameter(s):
        0: The module logic <OBJECT>
        1: Synchronized units <ARRAY>
        2: Activated <BOOL>

    Return Value:
        None
*/
#include "script_component.hpp"

params ["_logic", "_units", "_activated"];

if !(_activated && local _logic) exitWith {};

(missionNamespace getVariable ["bis_fnc_curatorObjectPlaced_mouseOver", [""]]) params ["_typeName", "_unit"];
if (_typeName != "OBJECT") then {
    ["Place on a unit or vehicle"] call ace_common_fnc_displayTextStructured;
} else {
    [[_unit, false, player]] remoteExecCall ["uksfCleanupToggle", 2];
};

deleteVehicle _logic;
