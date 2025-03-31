#include "cmdlineparser.h"

CmdLineParser::CmdLineParser (vector<CmdLineOption> options)
    :
    options_  {options}
{}

void CmdLineParser::processCmdLine (int argc, char** argv)
{
    for (int i = 1; i < argc; ++i)
        parseArgument (argv[i]);
}

void CmdLineParser::parseArgument (string argument)
{
    // See if the argument starts with double dash
    if (argument.substr (0, 2) != "--")
        // It doesn't, skip it
        return;
    
    // Remove the double dash
    argument = argument.substr (2, string::npos);

    // Split the argument in half, separated by the equal sign
    // The '-1' is what makes the regex split (-1 := what was not matched)
    regex re ("[=]");
    sregex_token_iterator first {argument.begin (), argument.end (), re, -1}, last;
    vector<string> tokens {first, last};

    removeEmptyTokens (tokens);

    processArgument ({tokens[0], tokens.size () > 1 ? tokens[1] : ""});
}

void CmdLineParser::processArgument (CmdLineArgument argument)
{
    // Find the option for this argument
    auto it1 = find_if (options_.begin (), options_.end (),
        [argument] (CmdLineOption option) { return option.name == argument.name; });

    if (it1 == options_.end ())
    {
        // Argument not found
        std::cout << "Unknown option: " << argument.name << std::endl;
        return;
    }

    // Argument found
    it1->func (argument.value);
}

void CmdLineParser::removeEmptyTokens (vector<string>& tokens)
{
    for (vector<string>::iterator it = tokens.begin (); it != tokens.end ();)
    {
        if (it->empty ())
            it = tokens.erase (it);
        else
            ++it;
    }
}
