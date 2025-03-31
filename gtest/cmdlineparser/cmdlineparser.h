#ifndef _COMMANDLINE_H_
#define _COMMANDLINE_H_

#include <regex>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <vector>

using std::regex;
using std::string;
using std::sregex_token_iterator;
using std::vector;
using std::find_if;
using std::vector;

// This class constitues an rudimentary command line parser, providing
// just enough functionality to be able to parse command line arguments
// such as "--option=value", without substantial error checking.
//
class CmdLineParser
{
public:
    struct CmdLineOption
    {
        string name;
        void (*func) (string value);
    };

    CmdLineParser (vector<CmdLineOption> options);
    void processCmdLine (int argc, char** argv);

private:
    struct CmdLineArgument
    {
        string name;
        string value;
    };

    vector<CmdLineOption> options_;

    void parseArgument (string argument);
    void processArgument (CmdLineArgument argument);
    void removeEmptyTokens (vector<string>& tokens);
};

#endif // _COMMANDLINE_H_