#include "console/console.h"

#include <queue>

using std::queue;

// Implementation of a Console class suited for unit in unit tests for the
// KD11ODT.
class TestConsole : public Console
{
public:
    // Console functions to be implemented
    void send (char) override;
    void setReceiver (function<void(int, char)>) override;
    bool isActive() override;
    void print (char const c) override;

    char getCharacter ();

private:
    std::function<void(int, char)> receiver_;
    queue<char> outputBuffer_;
};

// Send the given character to the DLV11J
inline void TestConsole::send (char c)
{
    receiver_ (3, c);
}

// The DLV11J reports us the function it wants to receive the input
// characters on.
inline void TestConsole::setReceiver (function<void(int, char)> receiver)
{
    receiver_ = receiver;
}

inline bool TestConsole::isActive ()
{
    return true;
}

// Put the character to be printed in the buffer, waiting fot it to be retrieved
// by a getCharacter() call.
inline void TestConsole::print (char const c)
{
    outputBuffer_.push (c);
}

// Get the next character waiting to be printed
char TestConsole::getCharacter ()
{
    char c;
    c = outputBuffer_.front ();
    outputBuffer_.pop ();
    return c;
}