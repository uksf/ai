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

class Extended_PostInit_EventHandlers {
    class ADDON {
        init = QUOTE(call COMPILE_FILE(XEH_postInit));
    };
};

class Extended_Init_EventHandlers {
    class All {
        class ADDON {
            killed = QUOTE([QGVAR(disableCache), _this] call CBA_fnc_serverEvent;);
        };
    };
    class B_UAV_AI {
        class ADDON {
            init = QUOTE((group (_this select 0)) setVariable [ARR_3(QQGVAR(excluded), true, true)];);
        };
    };
    class O_UAV_AI {
        class ADDON {
            init = QUOTE((group (_this select 0)) setVariable [ARR_3(QQGVAR(excluded), true, true)];);
        };
    };
    class I_UAV_AI {
        class ADDON {
            init = QUOTE((group (_this select 0)) setVariable [ARR_3(QQGVAR(excluded), true, true)];);
        };
    };
};
