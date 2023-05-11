#include "kd11odt.h"

using namespace KD11_ODT;

#include <sstream>
#include <iomanip>
#include <thread>
#include <limits>
#include <algorithm>

using std::stringstream;
using std::oct;
using std::setfill;
using std::setw;
using std::this_thread::sleep_for;
using std::numeric_limits;
using std::min;
using std::move;
using std::to_string;

KD11ODT::KD11ODT (Qbus *bus, KD11CPU &cpu)
    : 
    bus_ {bus},
    cpu_ {cpu},
    odtRunning_ {true},
    newValue_ {0},
    registerSeries_ {},
    location_ {}
{
    // Start the fsm by a transition to the AtPrompt_1 state
    dispatch (StartFsm {});
}



CondData<u8> KD11ODT::echoCharacter (CondData<u8> c)
{
    // All characters (except ASCII codes 0, 2, 010 and 012 <LF>) are to be
    // echoed (EK-KDJ1A-UG-001).
    // According to micro note 050 a <CR> has to be echoed as <CR><LF>.
    // An empty object is returned if the exit signal is set.
    if (c.hasValue ())
    {
        if (c != 0 && c != 2 && c != 010 && c != 012)
            character.write (c);
        if (c == '\r')
            character.write ('\n');
    }

    return c;
}



// Printing a new-line (\n) on a window in raw mode results for Windows
// in a <CR><LF> sequence and for Linux in just a <LF>. So to get the same
// output on both Windows and Linux, we have to add a <CR> for every <LF>
// in the string.
void KD11ODT::writeString (string str)
{
    for (u8 c : str)
    {
#ifdef __linux__
        if (c == '\n')
            write ('\r');
#endif // __linux__
        character.write (c);
    }
}

// Read the contents from the given address and write it to the console,
// indicating an error if the address could not be opened.
State KD11ODT::writeAddressContents (u16 address)
{
    if (bus_->read (address).hasValue ())
    {
        location_ = AddressLocation{ static_cast<u16> (address) };
        writeString (octalNumberToString (bus_->read (location_.address ())) + ' ');
        return AddressOpened_3{};
    }
    else
    {
        writeString ("?\n");
        return AtPrompt_1{};
    }
}

// The computer always prints six numeric characters [i.e. prints leading
// zero's]. (EK-11V03-TM-002)
string KD11ODT::octalNumberToString (u16 number)
{
    stringstream tmp {};
    tmp << oct << setfill ('0') << setw (6) << number;
    return tmp.str ();
}

u32 KD11ODT::stringToOctalNumber (string str)
{
    stringstream tmp {str};
    u32 returnValue;
    tmp >> oct >> returnValue;
    return returnValue;
}

// Convert the last n digits from the given octal number string to a u16
// and report the success or failure of this conversion.
//
// With the implementation of the RUBOUT the passed string can be empty. In 
// that case the string cannot be converted to a integer and an error is
// returned.
bool KD11ODT::stringTou16 (string str, size_t nDigits, u16 *value)
{
    if (str.empty ())
        return false;

    string lastNDigits = str.substr(str.size() - min (str.size(), nDigits));
    u32 tmp = stringToOctalNumber (lastNDigits);
    if (tmp > numeric_limits<u16>::max())
        return false;
    
    *value = static_cast<u16> (tmp);
    return true;
}

bool KD11ODT::registerSeriesEndsWith (string str)
{
    if (registerSeries_.size() >= 3)
    {
        return registerSeries_.substr (registerSeries_.size() - 3) == str;
    }
    else
        return false;
}

State KD11ODT::openAddress ()
{
    // Convert the last eight digits entered to an address. This can lead
    // to an address larger than the available memory, but currently the
    // address is a 16 bit unsigned integer and the maximum amount of memory
    // in a LSI-11 is 32Kw.
    if (u16 address; stringTou16 (digitSeries_, 8, &address))
    {
        // Clear the least significant bit in case an odd address is
        // specified
        return writeAddressContents (address & 0177776);
    }

    // Invalid address entered
    writeString ("?\n");
    return AtPrompt_1 {};
}

// Convert the last six digits entered to the new value. If this value
// exceeds the maximal value of a 16 bit word display an error.
void KD11ODT::setAddressValue ()
{
    if (stringTou16 (digitSeries_, 6, &newValue_))
        bus_->writeWord (location_.address (), newValue_);
    else
        writeString ("?\n");
}

// This function opens the next memory address. It is called from the
// states AddressOpened_3 and EnteringAddressValue_7.
// 
// Please note: When the user has the last bus address of a 32K word open
// segment and issues <LF>, ODT will open the first location of that segment.
// If the user wishes to cross the 32 K word boundary, the user must reenter
// the address for the desired 32 K word segment (i.e., ODT is modulo
// 32 K words). (EK-KDJ1A-UG-001)
// This is only applicable to the KDJ11-A. On a KDJ11-B and KDJ11-E ODT opens
// location 0 if the last location in the I/O page (17 777 776) is open and
// the user issues an <LF>. (EK-KDJ1B-UG-001)
// 
// The calculation of the next address has to be provided by the functor
// getNextAddress as there are a number of commands only differing on the
// calculation of the address.
// 
State KD11ODT::openNextAddress (std::function<u16(void)> getNextAddress)
{
    u16 address = getNextAddress ();
    writeString ("\n@" + octalNumberToString (address) + '/');
    return writeAddressContents (address);
}

// The processor registers will be incremented by one. If the PS is open when
// an <LF> is issued, it will be closed and <CR>, <LF>, @ will be printed;
// no new location will be opened. When a user has the last register, R7,
// open, and issues <LF>, ODT will "roll over" to the first register, RO.
// (EK-KDJ1A-UG-001)
//
// The calculation of the next register has to be provided by the functor
// getNextRegister. This functor should either increment of decrement the
// register number modulo 8 (the number of internal registers).
//
State KD11ODT::openNextRegister (State &&currentState, 
    std::function<u8(void)> getNextRegister)
{
    if (location_.isA<PSWLocation> ())
    {
        // As the <LF> isn't echoed the cursor is still at the end of the
        // line so move it to the start of the next line for the prompt to be
        // printed at the start of that line.
        writeString ("\n");
        return AtPrompt_1 {};
    }

    u8 registerNr = getNextRegister ();
    location_ = RegisterLocation {registerNr};
    writeString ("\n@R" + to_string(registerNr) + '/' + 
        octalNumberToString (cpu_.registerValue (registerNr)) + ' ');
    return move (currentState);
}

// Note that the contents of the PSW can be changed using the CR
// command but bit 4 (the T bit) cannot be set using any of the
// commands. (LSI11 PDP11/03 Processor Handbook)
void KD11ODT::setRegisterValue ()
{
    // Convert the last six digits entered to the new value. If this value
    // exceeds the maximal value of a 16 bit word display an error.
    if (stringTou16 (digitSeries_, 6, &newValue_))
    {
        // Either a register or the PSW has to be set to the new value.
        if (location_.isA<RegisterLocation> ())
            cpu_.setRegister (location_.registerNr (), newValue_);
        else
            // Setting or clearing the PSW T-bit will be prohibited
            // by setPSW()
            cpu_.setPSW (newValue_);
    }
    else
        writeString ("?\n");
}

// Process the given character in the state machine, returning true if we can
// accept another character, i.e. the state machine is still running.
bool KD11ODT::processCharacter (u8 character)
{
    dispatch (createEvent (echoCharacter (character)));
    return odtRunning_;
}
