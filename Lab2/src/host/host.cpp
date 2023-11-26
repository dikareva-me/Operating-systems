#include <syslog.h>
#include <stdexcept>
#include <exception>
#include "hostHandler.hpp"

int main() {
    openlog("HOST-LAB2", LOG_PID | LOG_NDELAY, LOG_USER);

    Host &host = Host::getInstance();

    try {
        host.run();

    } catch (std::runtime_error &error) {
        syslog(LOG_ERR, "%s", error.what());
        host.terminate();
        closelog();
        return EXIT_FAILURE;

    } catch (std::exception &error) {
        syslog(LOG_ERR, "undefined exception %s", error.what());
        host.terminate();
        closelog();
        return EXIT_FAILURE;
    }

    host.terminate();
    closelog();

    return EXIT_SUCCESS;
}