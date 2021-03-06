class CfgVehicles {
    class ACE_Module;
    class GVAR(moduleSettings): ACE_Module {
        scope = 2;
        displayName = QUOTE(Caching);
        icon = QPATHTOF(UI\Icon_Module_Caching_ca.paa);
        category = "UKSF";
        function = QFUNC(initModuleSettings);
        functionPriority = 1;
        isGlobal = 1;
        isTriggerActivated = 0;
        author = QUOTE(UKSF);
        class Arguments {
            class enabled {
                displayName = QUOTE(Caching);
                description = QUOTE(Enables AI Caching);
                typeName = "BOOL";
                defaultValue = 0;
            };
            class distance {
                displayName = QUOTE(Caching Distance);
                description = QUOTE(Sets caching distance in metres);
                typeName = "NUMBER";
                defaultValue = 750;
            };
        };
        class ModuleDescription {
            description = QUOTE(AI Caching settings);
        };
    };

    class uksf_common_moduleBase;
    class GVAR(moduleCachingToggle): uksf_common_moduleBase {
        curatorCanAttach = 1;
        isGlobal = 0;
        displayName = QUOTE(Caching toggle);
        function = QFUNC(moduleToggleCaching);
        icon = QPATHTOF(UI\Icon_Module_CachingToggle_ca.paa);
    };
};
