#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>
#include "IConn.hpp"
#include <iostream>
#include <syslog.h>

void IConn::openConn(size_t id, bool create) {
    _owner = create;
    _name = "/tmp/lab2_fifo";
    _id = -1;
    syslog(LOG_INFO, "[INFO] Create fifo by id %i name %s owner %i", (int)id, _name.c_str(), (int)_owner);
    if (_owner) {
        unlink(_name.c_str());
        if(mkfifo(_name.c_str(), 0666) == -1)
            throw std::runtime_error("mkfifo failed, error " + std::string(strerror(errno)));
    }
    _id  = open(_name.c_str(), O_RDWR);
    if (_id == -1) {
        throw std::runtime_error("mkfifo failed, error " + std::string(strerror(errno)));
    }
}

void IConn::readConn(void *buf, size_t size) {
    if(read(_id, (char*)buf, size) == -1){
        throw std::runtime_error("reading error " + std::string(strerror(errno)));
    }
}

void IConn::writeConn(void *buf, size_t size) {
    if(write(_id, (char*)buf, size) == -1){
        throw std::runtime_error("writing error " + std::string(strerror(errno)));
    }
}

void IConn::dropping(){ shift = 0; }

void IConn::closeConn() {
    if (close(_id) < 0 || (_owner && remove(_name.c_str()) < 0)) {
        throw std::runtime_error("closeConn error " + std::string(strerror(errno)));
    }
}