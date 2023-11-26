#include <stdexcept>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include "IConn.hpp"
#include <syslog.h>


void IConn::openConn(size_t id, bool create) {
    _owner = create;
    _name = "/lab2_seg";
    _id = -1;
    
    shift = 0;
    if (_owner) {
        _id = shmget(id, SIZE_MSG, IPC_CREAT | O_EXCL |  0666);
    } else {
        _id = shmget(id, SIZE_MSG, 0666);
    }
    if (_id == -1) {
        throw std::runtime_error("shmget failed, error " + std::string(strerror(errno)));
    }
    cur_buf = shmat(_id, 0, 0);
}

void IConn::dropping(){ shift = 0; }

void IConn::writeConn(void *buf, size_t size) {
    if (size + shift > SIZE_MSG)
        throw std::runtime_error("writing failed, error");
    
    memcpy(((char*)cur_buf + shift), buf, size);
    shift += size;
}

void IConn::readConn(void *buf, size_t size) {
    if (size + shift > SIZE_MSG)
        throw std::runtime_error("reading failed, error");
    memcpy(buf, ((char*)cur_buf + shift), size);
    shift += size;
}

void IConn::closeConn() {
    if(_owner && shmctl(_id, IPC_RMID, nullptr) < 0){
        throw std::runtime_error("close failed, error " + std::string(strerror(errno)));
    }
}