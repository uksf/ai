#pragma once

template <class T>
class threaded {
public:
    threaded() {
        uksf_ai::getInstance().missionEnded.connect([this]() {
            singleton<T>::getInstance().stopServerThread();
            singleton<T>::getInstance().stopClientThread();
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
    std::thread _serverThread, _clientThread;

    void startServerThread() {
        serverThreadStop = false;
        _serverThread = std::thread(&threaded<T>::serverThreadFunction, this);
        _serverThread.detach();
    };

    void startClientThread() {
        clientThreadStop = false;
        _clientThread = std::thread(&threaded<T>::clientThreadFunction, this);
        _clientThread.detach();
    };

    void stopServerThread() {
        if (_serverThread.joinable()) {
            serverThreadStop = true;
            _serverThread.join();
        }
    };

    void stopClientThread() {
        if (_clientThread.joinable()) {
            clientThreadStop = true;
            _clientThread.join();
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
