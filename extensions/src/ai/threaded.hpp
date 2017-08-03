#pragma once

#ifndef THREADED
#define THREADED
template <class T>
class threaded {
public:
    threaded() {
        uksf_ai::getInstance()->missionEnded.connect([this]() {
            singleton<T>::getInstance()->stopServerThread();
            singleton<T>::getInstance()->stopClientThread();
        });
    };

    virtual ~threaded() {
        stopServerThread();
        stopClientThread();
    };

    bool serverThreadStop = true;
    bool clientThreadStop = true;
    virtual void serverFunction() {};
    virtual void clientFunction() {};

protected:
    std::thread serverThread, clientThread;

    void startServerThread() {
        serverThreadStop = false;
        serverThread = std::thread(&threaded<T>::serverThreadFunction, this);
        serverThread.detach();
    };

    void startClientThread() {
        clientThreadStop = false;
        clientThread = std::thread(&threaded<T>::clientThreadFunction, this);
        clientThread.detach();
    };

    void stopServerThread() {
        if (serverThread.joinable()) {
            serverThreadStop = true;
            serverThread.join();
        }
    };

    void stopClientThread() {
        if (clientThread.joinable()) {
            clientThreadStop = true;
            clientThread.join();
        }
    };

    void serverThreadFunction() {
        while (!serverThreadStop) {
            if (uksf_ai_common::thread_run) {
                serverFunction();
            }
        }
    };

    void clientThreadFunction() {
        while (!clientThreadStop) {
            if (uksf_ai_common::thread_run) {
                clientFunction();
            }
        }
    };
};
#endif
