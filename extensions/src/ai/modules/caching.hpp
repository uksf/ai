#pragma once
#include "shared.hpp"
#include "uksf_ai.hpp"

class uksf_ai_caching {
public:
    uksf_ai_caching();

    static uksf_ai_caching* getInstance();

    void startClientThread();
    void stopClientThread();
    void onFrameFunction();

private:
    static uksf_ai_caching* instance;

    bool threadStop;
    std::thread clientThread;
    std::list<intercept::types::object> visibleUnits;

    void clientThreadFunction();
};
