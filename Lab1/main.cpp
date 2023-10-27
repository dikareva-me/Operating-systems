#include <iostream>
#include "daemon/Daemon.hpp"
#include <filesystem>


int main(){
    try {
   //     std::cout << "main:9\n";
        Daemon::getInstance().init("config.txt");

    //    std::cout << "main:12\n";
        Daemon::getInstance().run();
    } catch (std::exception const& e){
        syslog(LOG_ERR, "ERROR:: %s", e.what());
        Daemon::getInstance().terminate();
    }
    return 0;
}

