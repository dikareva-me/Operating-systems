#ifndef DAEMON_LAB_PARSER_H
#define DAEMON_LAB_PARSER_H

#include <string>
#include <map>
#include <list>
#include <vector>
#include <fstream>
#include <set>

class Parser {
public:
    class Grammar {
    public:
        enum ConfigParams{
            TIME_DELAY = 0,
            DIRECTORY1,
            DIRECTORY2,
            DEFAULT,
        };


        //how config file is written: names of params
        Grammar();
        //number of config params
        int getGrammarCount();
        //the value of parsed param
        ConfigParams getGrammarValue(const std::string& value);

    private:
        //the name and the value of config params
        const std::map<std::string, ConfigParams> _grammar;

        Grammar(Grammar const&) = delete;
        Grammar & operator=(Grammar const&) = delete;
    };

    Parser() = default;
    //get instance of a Parser class
    static Parser& getInstance();

    //parsing info from config file
    void parseConfig(std::string &configFileName);
    //get parsed config params
    std::map<Parser::Grammar::ConfigParams, std::string> getParams();

private:
    static Parser _instance;

    std::map<Parser::Grammar::ConfigParams, std::string> _config;
    Grammar _grammar;

    Parser(Parser const&) = delete;
    Parser & operator=(Parser const&) = delete;
};
#endif //DAEMON_LAB_PARSER_H