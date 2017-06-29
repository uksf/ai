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

class Extended_Killed_EventHandlers {
    class CAManBase {
        class ADDON {
            killed = "[(_this select 0)] remoteExecCall [""uksfCleanupKilled"", 2]";
        };
    };
    class Car {
        class ADDON {
            killed = "[(_this select 0)] remoteExecCall [""uksfCleanupKilled"", 2]";
        };
    };
    class Tank {
        class ADDON {
            killed = "[(_this select 0)] remoteExecCall [""uksfCleanupKilled"", 2]";
        };
    };
    class Air {
        class ADDON {
            killed = "[(_this select 0)] remoteExecCall [""uksfCleanupKilled"", 2]";
        };
    };
    class Ship {
        class ADDON {
            killed = "[(_this select 0)] remoteExecCall [""uksfCleanupKilled"", 2]";
        };
    };
};
