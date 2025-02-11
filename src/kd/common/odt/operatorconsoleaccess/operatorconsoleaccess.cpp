#include "operatorconsoleaccess.h"
#include "chrono/simulatorclock/simulatorclock.h"

using std::chrono::microseconds;

OperatorConsoleAccess::OperatorConsoleAccess (Bus* bus)
    : bus_ {bus}
{}

// Read a word from the given address. If dlv11j_.read() returns a
// StatusCode other than Success, the value 0 will be returned which will
// lead to an error transition in the command parser.
CondData<u16> OperatorConsoleAccess::readDLV11J (u16 address)
{
    return bus_->read (address);
}

// Determine if the DLV11J has a character available to be read
bool OperatorConsoleAccess::available ()
{
    return (readDLV11J (0177560) & 0x80) != 0;
}

// The console terminal addresses 17777560 through 17777566 are generated
// in microcode and cannot be changed (EK-KDJ1A-UG-001).
//
// Wait until a character is available and then read and return it
//
CondData<u8> OperatorConsoleAccess::read ()
{
    while (!available ())
        SimulatorClock::forwardClock (microseconds (50));

    // Read the character
    return static_cast<u8> (readDLV11J (0177562) & 0377);
}

// Determine if the DLV11J is ready to accept a character
bool OperatorConsoleAccess::transmitReady ()
{
    return (readDLV11J (0177564) & 0200) != 0;
}

void OperatorConsoleAccess::write (u8 c)
{
    while (!transmitReady ())
        SimulatorClock::forwardClock (microseconds (50));

    bus_->writeWord (0177566, static_cast<u8> (c));
}