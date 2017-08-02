class CfgVehicles {
    class ACE_Module;
    class GVAR(moduleSettings): ACE_Module {
        scope = 2;
        displayName = QUOTE(Cleanup);
        icon = QPATHTOF(UI\Icon_Module_Cleanup_ca.paa);
        category = "UKSF";
        function = QFUNC(initModuleSettings);
        functionPriority = 1;
        isGlobal = 1;
        isTriggerActivated = 0;
        author = QUOTE(UKSF);
        class Arguments {
            class enabled {
                displayName = QUOTE(Cleanup);
                description = QUOTE(Enables cleanup of dead units/vehicles);
                typeName = "BOOL";
                defaultValue = 0;
            };
            class delay {
                displayName = QUOTE(Cleanup Delay);
                description = QUOTE(Delay in seconds between death and cleanup);
                typeName = "NUMBER";
                defaultValue = 300;
            };
        };
        class ModuleDescription {
            description = QUOTE(AI Cleanup settings);
        };
    };

    class uksf_common_moduleBase;
    class GVAR(moduleCleanupToggle): uksf_common_moduleBase {
        curatorCanAttach = 1;
        displayName = QUOTE(Cleanup Toggle);
        function = QFUNC(moduleToggleCleanup);
        icon = QPATHTOF(UI\Icon_Module_CleanupToggle_ca.paa);
    };
};
