#include <iostream>
#include "daemon/Daemon.hpp"
#include <filesystem>


int main(){
    try {
        Daemon::getInstance().init("config.txt");

        Daemon::getInstance().run();
    } catch (std::exception const& e){
        syslog(LOG_ERR, "ERROR:: %s", e.what());
        Daemon::getInstance().terminate();
    }
    return 0;
}

