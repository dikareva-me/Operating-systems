#ifndef LAB2_CLIENT_H
#define LAB2_CLIENT_H

#include <csignal>
#include <semaphore.h>
#include "../conn/IConn.hpp"
#include "../utils/safeQueue.h"

#define MAX_MESSAGE_LEN 400

class Client {
public:

    static  Client &getInstance();
    void run();
    void terminate();
    void setHostPid(int hostPid);
    bool sendMessages(ConnInfo& info);
    bool getMessages(ConnInfo& info);
    static std::string getFromTerminal();
    static void sendFromTerminal();
    static bool isRunning(void) { return getInstance()._isRunning; }
private:
    static Client _instance;
    SafeQueue _inputMsg;
    SafeQueue _outputMsg;
    ConnInfo info;
    int _host_pid;

    bool _isRunning = true;

    void openConnection(ConnInfo& info);
    void processConn();
    
    explicit Client();
    static void handleSignal(int signum, siginfo_t *info, void *ptr);

    Client(Client &) = delete;
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;
};


#endif //LAB2_CLIENT_H