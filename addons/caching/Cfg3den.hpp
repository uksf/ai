class Cfg3DEN {
    class Group {
        class AttributeCategories {
            class uksf_attributes {
                class Attributes {
                    class GVAR(excluded) {
                        property = QGVAR(excluded);
                        control = "Checkbox";
                        displayName = QUOTE(Caching exclude);
                        tooltip = QUOTE(Excludes group from caching);
                        expression = "if (_value) then {[{[[(_this select 0), true, objNull]] remoteExecCall [""uksfCachingToggle"", 2];},[_this]] call cba_fnc_execNextFrame};";
                        typeName = "BOOL";
                        defaultValue = "(false)";
                    };
                };
            };
        };
    };
};
