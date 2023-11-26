#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <cstring>
#include <mqueue.h>
#include "IConn.hpp"
#include <iostream>
#include <syslog.h>



void IConn::openConn(size_t id, bool create) {
    _owner = create;
    _name = "/lab2_mq" + std::to_string(id);
    _id = -1;

    if (_owner) {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 10;
        attr.mq_curmsgs = 0;
        attr.mq_msgsize = SIZE_MSG;
        _id = mq_open(_name.c_str(),  O_CREAT | O_RDWR, 0666, &attr);
    } else {
        _id = mq_open(_name.c_str(), O_RDWR);
    }
    if (_id == -1) {
        throw std::runtime_error("mq_open failed, error " + std::string(strerror(errno)));
    }
    syslog(LOG_INFO, "[INFO] Opened conn id = %i ", _id);
}

void IConn::readConn(void *buf, size_t size) {
    if(mq_receive(_id, (char*)buf, SIZE_MSG, 0) == -1){
        throw std::runtime_error("reading error " + std::string(strerror(errno)));
    }
}

void IConn::writeConn(void *buf, size_t size) {
    if(mq_send(_id, (char*)buf, size, 0) == -1){
        throw std::runtime_error("writing error " + std::string(strerror(errno)));
    }
}

void IConn::dropping(){ }

void IConn::closeConn() {
    if(mq_close(_id) == -1){
        throw std::runtime_error("writing error " + std::string(strerror(errno)));
    }
    if(_owner && mq_unlink(_name.c_str()) != 0){
        throw std::runtime_error("closeConn error " + std::string(strerror(errno)));
    }
}