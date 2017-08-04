#pragma once
#include "common.hpp"

#define CLEANUP_ENABLED_DEFAULT true
#define CLEANUP_DELAY_DEFAULT 300

typedef std::tuple<object, clock_t, bool> killed_map_type;

class uksf_ai_cleanup : public singleton<uksf_ai_cleanup>, public threaded<uksf_ai_cleanup> {
public:
    uksf_ai_cleanup();
    ~uksf_ai_cleanup();

    virtual void serverFunction();
    virtual void clientFunction() {};

private:
    std::unordered_map<size_t, killed_map_type> killedMap{};
    bool cleanupEnabled = CLEANUP_ENABLED_DEFAULT;
    float cleanupDelay = CLEANUP_DELAY_DEFAULT;

    types::registered_sqf_function uksfCleanupKilled;
    static game_value uksfCleanupKilledFunction(game_value param);
    types::registered_sqf_function uksfCleanupToggle;
    static game_value uksfCleanupToggleFunction(game_value params);
};
