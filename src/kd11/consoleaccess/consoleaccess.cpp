#include "consoleaccess.h"

using std::this_thread::sleep_for;

ConsoleAccess::ConsoleAccess (Qbus* bus)
    : bus_ {bus}
{}

// Read a word from the given address. If dlv11j_.read() returns a
// StatusCode other than OK, the value 0 will be returned which will
// lead to an error transition in the command parser.
CondData<u16> ConsoleAccess::readDLV11J (u16 address)
{
    return bus_->read (address);
}

// Determine if the DLV11J has a character available to be read
bool ConsoleAccess::available ()
{
    return (readDLV11J (0177560) & 0x80) != 0;
}

// The console terminal addresses 17777560 through 17777566 are generated
// in microcode and cannot be changed (EK-KDJ1A-UG-001).
//
// Wait until a character is available and then read and return it
//
CondData<u8> ConsoleAccess::read ()
{
    while (!available ())
        sleep_for (std::chrono::milliseconds (50));

    // Read the character
    return (readDLV11J (0177562));
}

void ConsoleAccess::write (u8 c)
{
    // ToDo: Check for transmit ready?
    bus_->writeWord (0177566, static_cast<u8> (c));
}