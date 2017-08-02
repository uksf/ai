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
                        expression = "if (_value) then {[{[[(_this select 0)]] remoteExecCall [""uksfCachingToggle"", 0];},[_this]] call CBA_fnc_execNextFrame};"; // Must be executed globally
                        typeName = "BOOL";
                        defaultValue = "(false)";
                    };
                };
            };
        };
    };
};
