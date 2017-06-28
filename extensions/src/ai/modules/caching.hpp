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

    static bool cachingEnabled;
    static float cachingDistance;

private:
    bool serverThreadStop = true;
    bool clientThreadStop = true;
    std::thread serverThread, clientThread;
    std::list<types::object> visibleUnits;

    void serverThreadFunction();
    void clientThreadFunction();
};
