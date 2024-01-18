#ifndef _TESTCONSOLEACCESS_H
#define _TESTCONSOLEACCESS_H_

#include "kd/include/consoleaccess.h"

#include <string>
#include <queue>
#include <algorithm>

using std::string;
using std::queue;
using std::copy;

// Implementation of a ConsoleAccess class used by KD11ODt for test purposes.
class TestConsoleAccess : public ConsoleAccess
{
public:
    TestConsoleAccess (string inputString);

    // Declaration of required functions
    bool available ();
	CondData<u8> read ();
    void write (u8 c);

    // Additional functions, to be used by the unit tests
    char getCharacter ();

private:
    string inputBuffer_;
    string::iterator inputIter_;
    queue<char> outputBuffer_;
};

TestConsoleAccess::TestConsoleAccess (string inputString)
    :
    inputBuffer_ {move (inputString)},
    inputIter_ {inputBuffer_.begin ()}
{}

inline bool TestConsoleAccess::available ()
{
    return inputIter_ < inputBuffer_.end ();
}

// Read a character from the test console. The function gets the character
// from the supplied string.
CondData<u8> TestConsoleAccess::read ()
{
    return *inputIter_++;
}

// Write a character to the test console. The function put the character in
// an outputbuffer from which it can be retrieved by getCharacter().
void TestConsoleAccess::write (u8 c)
{
    outputBuffer_.push (c);
}

// Get the next character waiting to be written to the console
char TestConsoleAccess::getCharacter ()
{
    char c;
    c = outputBuffer_.front ();
    outputBuffer_.pop ();
    return c;
}

#endif _TESTCONSOLEACCESS_H_