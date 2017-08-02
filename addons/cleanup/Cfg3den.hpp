class Cfg3DEN {
    class Object {
        class AttributeCategories {
            class uksf_attributes {
                class Attributes {
                    class GVAR(excluded) {
                        property = QGVAR(excluded);
                        control = "Checkbox";
                        displayName = QUOTE(Cleanup exclude);
                        tooltip = QUOTE(Excludes object from cleanup);
                        expression = "[[_this, true]] remoteExecCall [""uksfCleanupToggle"", 2]";
                        typeName = "BOOL";
                        defaultValue = "(false)";
                        condition = "1";
                    };
                };
            };
        };
    };
};
