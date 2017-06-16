#pragma once
#include "common.hpp"
#include "uksf_ai.hpp"

class uksf_ai_caching: public singleton<uksf_ai_caching> {
public:
    uksf_ai_caching();

    void startServerThread();
    void startClientThread();
    void stopServerThread();
    void stopClientThread();
    void onFrameFunction();

    const bool& getCachingEnabled();
    void setCachingEnabled(const bool& value);
    const float& getCachingDistance();
    void setCachingDistance(const float& value);

private:
    bool serverThreadStop, clientThreadStop;
    std::thread serverThread, clientThread;
    std::list<types::object> visibleUnits;

    void serverThreadFunction();
    void clientThreadFunction();

    bool cachingEnabled;
    float cachingDistance;
};
