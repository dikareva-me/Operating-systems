#ifndef LAB2_ICONN_H
#define LAB2_ICONN_H
#include <cstddef>
#include <string>
#include <semaphore.h>
#include "string.h"

#define SIZE_MSG 1024
#define MAXLEN 400

class IConn {
public:
    IConn(){}
    IConn(pid_t clientPid, bool isCreator)
    {
        _owner = isCreator;
        _name = "/tmp/conn_" + std::to_string(clientPid);
    }
    
    class Message{
    public:
        Message(const std::string& tmp = "default"){
            strncpy(this->msg, tmp.c_str(), std::max((int)tmp.size(), (int)MAXLEN));
        }
        void setStr(const std::string& str){
            strncpy(this->msg, str.c_str(), std::max((int)str.size(), (int)MAXLEN));
        }
        char msg[MAXLEN];
    };

    static IConn* getConnection() {
        static IConn connMq;
        return &connMq;
    }
    void openConn(size_t id, bool create);
    void readConn(void *buf, size_t size);
    void writeConn(void *buf, size_t size);
    void dropping();
    void closeConn();

private:
    int _id;
    bool _owner;
    int shift;
    void *cur_buf;
    std::string _name;
};

class  ConnInfo{
public:
    IConn *conn;
    sem_t *_sem_client;
    sem_t *_sem_host;
};

#endif 