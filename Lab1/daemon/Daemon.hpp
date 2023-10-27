#ifndef DAEMON_LAB_DAEMON_H
#define DAEMON_LAB_DAEMON_H

#include <csignal>
#include <unistd.h>
#include <sys/stat.h>
#include <syslog.h>
#include <exception>
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <regex>
#include "Parser.hpp"

#endif //DAEMON_LAB_DAEMON_H
using std::string;

class Daemon{
public:
    Daemon(Daemon const&) = delete;
    
    static Daemon& getInstance();

    void init(const std::string &config);
    void run();
    static void signalManager(int signalNum);
    void initSignals();
    void terminate();
    
private:
    Daemon(){}
    
    void setConfig(const std::string &configFile);
    void walkThroughFile(const string& path);
    string getAbsolutePath(const string &path) const;
    void loadConfig();

    bool initTread() const;
    bool initPid() const;
    
    void checkPid() const;
    void savePid() const;

    void copyContent(const string& filePath) const;
    bool isLogFile(const string& file) const;
    
    const string targetFileFormat = "log";
    string _inputDir, _outputDir;
    string totalLogPath;
    uint32_t _sleepTime;
    string _homeDir, _configFile;
    bool _isRunning = false;
    string _pidFilePath = "/var/run/lab1.pid";
};