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
    class B_UAV_AI {
        class ADDON {
            init = "[[(_this select 0), true, objNull]] remoteExecCall [""uksfCachingToggle"", 2]";
        };
    };
    class O_UAV_AI {
        class ADDON {
            init = "[[(_this select 0), true, objNull]] remoteExecCall [""uksfCachingToggle"", 2]";
        };
    };
    class I_UAV_AI {
        class ADDON {
            init = "[[(_this select 0), true, objNull]] remoteExecCall [""uksfCachingToggle"", 2]";
        };
    };
};

class Extended_Killed_EventHandlers {
    class All {
        class ADDON {
            killed = "[[(_this select 0), true, objNull]] remoteExecCall [""uksfCachingToggle"", 2]";
        };
    };
};
