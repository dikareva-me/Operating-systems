#include <iostream>
#include "daemon/Daemon.hpp"
#include <filesystem>


int main(int argc, char **argv){
    try {
        std::string config_file = argc > 1 ? argv[1] : "config.txt"; 
        Daemon::getInstance().init(config_file);
        Daemon::getInstance().run();
    } catch (std::exception const& e){
        syslog(LOG_ERR, "ERROR:: %s", e.what());
        Daemon::getInstance().terminate();
    }
    return 0;
}