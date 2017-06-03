class CfgFactionClasses {
    class UKSF_VR {
        displayName = "Virtual Army";
        icon = "ca\ca_pmc\flag_ion_white_co";
        priority = 1;
        side = 0;
    };
    class NO_CATEGORY;
    class GVAR(zeus_uksf): NO_CATEGORY {
        displayName = "UKSF";
        priority = 2;
        side = 7;
    };
    class GVAR(zeus_uksf_special): GVAR(zeus_uksf) {
        displayName = "UKSF Special";
    };
};
