#include <windows.h>
#include <stdio.h>
#include <cstdint>
#include <atomic>
#include "uksf_ai.hpp"

#include "intercept.hpp"
#include "logging.hpp"

#include "SignalSlot.hpp"

INITIALIZE_EASYLOGGINGPP

int __cdecl intercept::api_version() {
    return 1;
}

void __cdecl intercept::pre_init() {
    uksf_ai::getInstance().preInit.emit();
}

void __cdecl intercept::post_init() {
    uksf_ai::getInstance().postInit.emit();
}

uksf_ai::uksf_ai() {
    preInit.connect([]() {
        intercept::sqf::diag_log("UKSF AI SIGNAL PREINIT"_sv);
    });

    postInit.connect([]() {
        intercept::sqf::diag_log("UKSF AI SIGNAL POSTINIT"_sv);
    });
}
uksf_ai::~uksf_ai() {}

uksf_ai& uksf_ai::getInstance() {
    static uksf_ai instance;
    return instance;
}

void init(void) {
    el::Configurations conf;

    conf.setGlobally(el::ConfigurationType::Filename, "logs/uksf_ai_dll.log");
    conf.setGlobally(el::ConfigurationType::MaxLogFileSize, "10240");
#ifdef _DEBUG
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "[%datetime] - %level - {%loc}t:%thread- %msg");
    conf.setGlobally(el::ConfigurationType::PerformanceTracking, "true");
#else
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%datetime-{%level}- %msg");
#endif
    el::Loggers::setDefaultConfigurations(conf, true);

    LOG(INFO) << "Intercept UKSF AI DLL Loaded";
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        init();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
