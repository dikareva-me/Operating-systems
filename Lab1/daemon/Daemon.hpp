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
    void operator=(Daemon const&)  = delete;
    
    //get instance of daemon class
    static Daemon& getInstance();

    //handle incoming signals
    static void signalManager(int signalNum);
    //set signals
    void initSignals();
    //terminate daemon
    void terminate();
    // initialize daemon instance with config file
    void init(const std::string &config);
    //start daemon work cycle
    void run();
    
private:
    Daemon(){}
    
    //set absolute path to config file
    void setConfig(const std::string &configFile);
    //iterating through files in the input directory
    void walkThroughFile(const string& path);
    //get parsed configs from Parser
    void loadConfig();
    //get absolute path from relative path
    string getAbsolutePath(const string &path) const;

    //initiating daemon in the system
    bool forkProcess() const;
    bool initPid() const;
    
    //check pid file if daemon is already working
    void checkPid() const;
    //write our process's pid to pid file
    void savePid() const;
    //copy contents of a log file and prepare it for writing in total.log
    void copyContent(const string& filePath) const;
    //check if file is *.log
    bool isLogFile(const string& file) const;
    
    const string targetFileFormat = "log";
    string _inputDir, _outputDir;
    string totalLogPath;
    uint32_t _sleepTime;
    string _homeDir, _configFile;
    bool _isRunning = false;
    string _pidFilePath = "/var/run/lab1.pid";
};