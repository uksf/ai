/*
    Author:
        Tim Beswick

    Description:
        Toggles unit cache state

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
if (_typeName != "OBJECT" && {!(alive _unit)}) then {
    ["Place on a living unit or vehicle"] call ace_common_fnc_displayTextStructured;
} else {
    private _unit = effectivecommander _unit;
    if !(_unit isKindOf "CAManBase") then {
        ["Unit must be infantry"] call ace_common_fnc_displayTextStructured;
    } else {
        [[_unit, false, player]] remoteExecCall ["uksfCachingToggle", 2];
    };
};

deleteVehicle _logic;
