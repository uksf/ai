#pragma once
#include "common.hpp"

class uksf_ai_spawning : public singleton<uksf_ai_spawning>, public threaded<uksf_ai_spawning> {
public:
    uksf_ai_spawning() {
        uksf_ai::getInstance()->preStart.connect([this]() {
            LOG(DEBUG) << "SPAWNING PRESTART";

        });

        uksf_ai::getInstance()->preInit.connect([this]() {
            LOG(DEBUG) << "SPAWNING PREINIT";

        });

        uksf_ai::getInstance()->postInit.connect([this]() {
            LOG(DEBUG) << "SPAWNING POSTINIT";

        });

        uksf_ai::getInstance()->missionEnded.connect([this]() {
            LOG(DEBUG) << "SPAWNING MISSION ENDED";

        });
    };

    ~uksf_ai_spawning() {

    };

private:
    
};
