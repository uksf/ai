#pragma once
#include "common.hpp"
#include "uksf_ai.hpp"

#ifdef COMPONENT
#undef COMPONENT
#define COMPONENT caching
#endif
#include "macros.hpp"

class uksf_ai_caching: public singleton<uksf_ai_caching> {
public:
    uksf_ai_caching();

    void startClientThread();
    void startServerThread();
    void stopClientThread();
    void stopServerThread();
    void onFrameFunction();

private:
    bool clientThreadStop, serverThreadStop;
    std::thread clientThread, serverThread;
    std::list<types::object> visibleUnits;

    void clientThreadFunction();
    void serverThreadFunction();
};
