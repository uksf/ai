#include "script_component.hpp"

class CfgPatches {
    class ADDON {
        name = COMPONENT_NAME;
        units[] = {};
        weapons[] = {};
        requiredVersion = REQUIRED_VERSION;
        requiredAddons[] = {
            "Intercept_Core"
        };
        author = QUOTE(UKSF);
        url = QUOTE(www.uk-sf.com);
        VERSION_CONFIG;
    };
};

class CfgMods {
    class Mod_Base;
    class PREFIX: Mod_Base {
        dir = "@uksf_ai";
        name = "UKSF AI";
        author = "UKSF";
        picture = QPATHTO_R(data\uksf_logo_ca);
        logo = QPATHTO_R(data\uksf_logo_ca);
        logoOver = QPATHTO_R(data\uksf_logo_ca);
        logoSmall = QPATHTO_R(data\uksf_logo_ca);
        hideName = 0;
        hidePicture = 0;
        actionName = "Website";
        action = "http://uk-sf.com/";
        description = "Issue Tracker: https://github.com/uksf/ai/issues";
        tooltip = "UKSF AI";
        tooltipOwned = "UKSF AI - Owned";
        overview = "Custom AI mod made with Intercept";
    };
};

#include "CfgSettings.hpp"
#include "CfgIntercept.hpp"
