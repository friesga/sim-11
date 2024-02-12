#ifndef _CONSOLECONFIG_H_
#define _CONSOLECONFIG_H_

struct ConsoleConfig
{
    static const unsigned char escapeKey {27};
    enum class BreakResponse
    {
        Boot,
        Halt,
        None
    };

    BreakResponse breakResponse {BreakResponse::Halt};
    unsigned char breakKey {escapeKey};
};

#endif // _CONSOLECONFIG_H_