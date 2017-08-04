#pragma once
#include "common.hpp"

#define CACHING_ENABLED_DEFAULT true
#define CACHING_DISTANCE_DEFAULT 750
#define CACHING_TIME_SERVER 10
#define CACHING_TIME_CLIENT 5

typedef std::tuple<group, clock_t> cache_map_type;

class uksf_ai_caching : public singleton<uksf_ai_caching>, public threaded<uksf_ai_caching> {
public:
    uksf_ai_caching();
    ~uksf_ai_caching();

    virtual void serverFunction();
    virtual void clientFunction();

private:
    std::unordered_map<size_t, cache_map_type> cacheMap{};
    bool cachingEnabled = CACHING_ENABLED_DEFAULT;
    float cachingDistance = CACHING_DISTANCE_DEFAULT;

    types::registered_sqf_function uksfCachingAdd;
    static game_value uksfCachingAddFunction(game_value param);
    types::registered_sqf_function uksfCachingRemove;
    static game_value uksfCachingRemoveFunction(game_value param);
    types::registered_sqf_function uksfCachingUpdate;
    static game_value uksfCachingUpdateFunction(game_value params);
    types::registered_sqf_function uksfCachingToggle;
    static game_value uksfCachingToggleFunction(game_value params);
}; 
