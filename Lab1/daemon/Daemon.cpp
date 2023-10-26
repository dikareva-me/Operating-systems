#include "Daemon.hpp"


Daemon& Daemon::getInstance(){
        static Daemon _instance;
        return _instance;
}

string Daemon::getAbsolutePath(const string &path) const{
    if (path.empty() || path[0] == '/') {
        return path;
    }
    string output = _homeDir + "/" + path;
    char *result = realpath(output.c_str(), nullptr);
    if (result == nullptr) {
        throw std::runtime_error("ERROR: Couldn't find path:" + path);
    }
    string resultPath(result);
    free(result);
    return resultPath;
}

void Daemon::init(const std::string &config) {
    openlog("daemonlab1", LOG_PID|LOG_NDELAY, LOG_USER);
    syslog(LOG_INFO, "Initializing daemon");
    std::cout << "Initializing daemon\n";

    char buf[FILENAME_MAX];
    if (!getcwd(buf, FILENAME_MAX)) {
        throw std::runtime_error("ERROR: Failed in getcwd");

    std::cout << "ERROR: Failed in getcwd\n";
    }
    _homeDir = buf;
    syslog(LOG_INFO, "Home directory - %s", buf);

    std::cout << "Home directory - " << buf <<'\n';

 
    syslog(LOG_INFO, "Daemon is successfully initialized");
    _isRunning = true;
}

void Daemon::setConfig(const std::string &configFile) {
    _configFile = getAbsolutePath(configFile);
    if (_configFile.empty()) {
        throw std::runtime_error("ERROR: Wrong config file full path " +  _configFile);
    }
    syslog(LOG_INFO, "Trying read config file - %s", _configFile.c_str());
}


void Daemon::run(){
    while (_isRunning) {
        walkThroughFile(_inputDir);
        sleep(_sleepTime);
    }
}

void Daemon::walkThroughFile(const string& path){
    for (auto&& file : std::filesystem::directory_iterator(path)) {
        string filePath = file.path().string();

        if (std::filesystem::is_directory(file)) {
            walkThroughFile(filePath);
        }
        else if(isLogFile(filePath)){
            try{
                copyContent(filePath);
            }
            catch(std::exception &ex){
                std::cerr << ex.what();
            }
        }
    }
}

void Daemon::copyContent(const string& filePath) const{
        if(!std::filesystem::exists(filePath)) {
            throw std::runtime_error("ERROR: Undefined file path");
        }
        std::ofstream fout(totalLogPath, std::ios_base::out | std::ios_base::app);
        std::ifstream fin(filePath);
        fout << "\n\n" << filePath + "\n" << std::endl;

        string line;
        while (std::getline(fin, line)) {
            fout << line + "\n";
        }
        fin.close();

        if(!std::filesystem::remove(filePath)){
            throw std::runtime_error("ERROR: deleting a file");
        }
}

bool Daemon::isLogFile(const string& file) const{
    std::regex reg("." + targetFileFormat + "$");
    return regex_search(file, reg);
}