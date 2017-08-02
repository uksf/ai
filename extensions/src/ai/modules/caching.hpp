#pragma once
#include "common.hpp"
#include "uksf_ai.hpp"

#define CACHING_ENABLED_DEFAULT true
#define CACHING_DISTANCE_DEFAULT 750
#define CACHING_TIME_SERVER 10
#define CACHING_TIME_CLIENT 5

typedef std::tuple<group, clock_t> cache_map_type;

class uksf_ai_caching: public singleton<uksf_ai_caching> {
public:
    uksf_ai_caching();
    ~uksf_ai_caching();

    void startServerThread();
    void startClientThread();
    void stopServerThread();
    void stopClientThread();

    static bool cachingEnabled;
    static float cachingDistance;

    std::unordered_map<size_t, cache_map_type> cacheMap;

    types::registered_sqf_function uksfCachingAdd;
    static game_value uksfCachingAddFunction(game_value param);
    types::registered_sqf_function uksfCachingRemove;
    static game_value uksfCachingRemoveFunction(game_value param);
    types::registered_sqf_function uksfCachingUpdate;
    static game_value uksfCachingUpdateFunction(game_value params);
    types::registered_sqf_function uksfCachingToggle;
    static game_value uksfCachingToggleFunction(game_value params);

private:
    static bool serverThreadStop;
    static bool clientThreadStop;
    std::thread serverThread, clientThread;

    void serverThreadFunction();
    void clientThreadFunction();
};
