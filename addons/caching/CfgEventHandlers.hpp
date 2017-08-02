class Extended_PreStart_EventHandlers {
    class ADDON {
        init = QUOTE(call COMPILE_FILE(XEH_preStart));
    };
};

class Extended_PreInit_EventHandlers {
    class ADDON {
        init = QUOTE(call COMPILE_FILE(XEH_preInit));
    };
};

class Extended_Init_EventHandlers {
    class CAManBase {
        class ADDON {
            init = "[{uksfCachingAdd _this}, (_this select 0), 5] call CBA_fnc_waitAndExecute;"; // Globally executed
            exclude[] = { "UAV_AI_base_F", "B_UAV_AI", "O_UAV_AI", "I_UAV_AI" };
        };
    };
};

class Extended_Killed_EventHandlers {
    class CAManBase {
        class ADDON {
            killed = "[(_this select 0)] remoteExecCall [""uksfCachingRemove"", 0]"; // Must be executed globally
        };
    };
};
