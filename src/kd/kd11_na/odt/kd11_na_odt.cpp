#include "kd11_na_odt.h"
#include "trace/trace.h"

#include <sstream>
#include <iomanip>

using std::stringstream;
using std::oct;
using std::setfill;
using std::setw;
using std::to_string;
using std::make_unique;
using std::move;

KD11_NA_ODT::KD11_NA_ODT (Qbus *bus, CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, unique_ptr<ConsoleAccess> consoleAccess)
    : 
    bus_ {bus},
    cpu_ {cpu},
    cpuControl_ {cpuControl},
    mmu_ {mmu},
    console_ {move (consoleAccess)},
    odtRunning_ {true},
    newValue_ {0},
    registerSeries_ {},
    location_ {}
{
    stateMachine_ = make_unique<StateMachine> (this);

    // Start the fsm by a transition to the AtPrompt_1 state
    stateMachine_->dispatch (StartFsm {});
}

unique_ptr<KD11_NA_ODT> KD11_NA_ODT::createODT (Qbus *bus, CpuData* cpu,
    CpuControl* cpuControl, MMU* mmu, unique_ptr<ConsoleAccess> consoleAccess)
{
    return move (make_unique<KD11_NA_ODT> (bus, cpu, cpuControl, mmu,
        make_unique<OperatorConsoleAccess> (bus)));
}

CondData<u8> KD11_NA_ODT::echoCharacter (CondData<u8> c)
{
    // All characters (except ASCII codes 0, 2, 010 <BS> and 012 <LF>) are
    // to be echoed (EK-KDJ1A-UG-001).
    // 
    // According to micro note 050 a <CR> has to be echoed as <CR><LF>.
    // Test runs conducted at a real LSI-11/2 demonstrated that <CR> is echoed
    // as just <CR> without a <LF> and that echoing of a <LF> is not
    // suppressed.
    // 
    // An empty object is returned if the exit signal is set.
    //
    // For the echoing of characters applies the same as for printing
    // characters: writing a new-line on Linux results in the output
    // of just <LF>, while on Windows it results in the  of <CR><LF>.
    // Therefore writing a new-line on Linux is preceded by a <CR>.
    //
    if (c.hasValue ())
    {
#ifdef __linux__
        if (c == '\n')
            console_->write ('\r');
#endif // __linux__

        if (c != 0 && c != 2 && c != 010)
            console_->write (c);
    }

    return c;
}

// Printing a new-line (\n) on a window in raw mode results for Windows
// in a <CR><LF> sequence and for Linux in just a <LF>. So to get the same
// output on both Windows and Linux, we have to add a <CR> for every <LF>
// in the string.
void KD11_NA_ODT::writeString (string str)
{
    for (u8 c : str)
    {
#ifdef __linux__
        if (c == '\n')
            console_->write ('\r');
#endif // __linux__
        console_->write (c);
    }
}

// Read the contents from the given address and write it to the console,
// indicating an error if the address could not be opened.
//
// Test runs conducted on a real LSI-11/2 show the last opened location
// is set too on opening an invalid address.
KD11_NA_ODT::State KD11_NA_ODT::writeAddressContents (u16 address)
{
    location_ = AddressLocation {static_cast<u16> (address)};
    if (bus_->read (address).hasValue ())
    {
        writeString (octalNumberToString (bus_->read (location_.wordAddress ())) + ' ');
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
string KD11_NA_ODT::octalNumberToString (u16 number)
{
    stringstream tmp {};
    tmp << oct << setfill ('0') << setw (6) << number;
    return tmp.str ();
}

u32 KD11_NA_ODT::stringToOctalNumber (string str)
{
    stringstream tmp {str};
    u32 returnValue;
    tmp >> oct >> returnValue;
    return returnValue;
}

// Convert the least significant 16 bits from the given octal number
// string to a u16 and report the success or failure of this conversion.
//
// With the implementation of the RUBOUT the passed string can be empty. In 
// that case the string cannot be converted to a integer and an error is
// returned.
bool KD11_NA_ODT::stringTou16 (string str, size_t nDigits, u16 *value)
{
    if (str.empty ())
        return false;

    u32 tmp = stringToOctalNumber (str);
    *value = static_cast<u16> (tmp & 0177777);
    return true;
}

bool KD11_NA_ODT::registerSeriesEndsWith (string str)
{
    if (registerSeries_.size() >= 3)
    {
        return registerSeries_.substr (registerSeries_.size() - 3) == str;
    }
    else
        return false;
}

KD11_NA_ODT::State KD11_NA_ODT::openAddress ()
{
    // Convert the last eight digits entered to an address. This can lead
    // to an address larger than the available memory, but currently the
    // address is a 16 bit unsigned integer and the maximum amount of memory
    // in a LSI-11 is 32kW.
    u16 address {0};
    stringTou16 (digitSeries_, 8, &address);
    
    // The specified address might be a byte or a word address. This is
    // dealt with in the Location class.
    return writeAddressContents (address);
}

// Convert the least significant 16 bits of the value entered to the new
// value. If this value exceeds the maximal value of a 16 bit word display an error.
void KD11_NA_ODT::setAddressValue ()
{
    if (stringTou16 (digitSeries_, 6, &newValue_))
        bus_->writeWord (location_.wordAddress (), newValue_);
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
KD11_NA_ODT::State KD11_NA_ODT::openNextAddress (std::function<u16(void)> getNextAddress)
{
    u16 address = getNextAddress ();
    writeString (octalNumberToString (address) + '/');
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
KD11_NA_ODT::State KD11_NA_ODT::openNextRegister (State &&currentState, 
    std::function<u8(void)> getNextRegister)
{
    if (location_.isA<PSWLocation> ())
        return AtPrompt_1 {};

    u8 registerNr = getNextRegister ();
    location_ = RegisterLocation {registerNr};
    writeString ("R" + to_string(registerNr) + '/' + 
        octalNumberToString (cpu_->registers ()[registerNr]) + ' ');
    return move (currentState);
}

// Note that the contents of the PSW can be changed using the CR
// command but bit 4 (the T bit) cannot be set using any of the
// commands. (LSI11 PDP11/03 Processor Handbook)
void KD11_NA_ODT::setRegisterValue ()
{
    // Convert the last six digits entered to the new value. If this value
    // exceeds the maximal value of a 16 bit word display an error.
    if (stringTou16 (digitSeries_, 6, &newValue_))
    {
        // Either a register or the PSW has to be set to the new value.
        if (location_.isA<RegisterLocation> ())
            cpu_->registers ()[location_.registerNr ()] = newValue_;
        else
            // Setting or clearing the PSW T-bit will be prohibited
            // by setPSW()
            cpuControl_->setPSW (newValue_);
    }
    else
        writeString ("?\n");
}

// Set the CPU into the running state with the specified address as the PC
void KD11_NA_ODT::startCPU (u16 address)
{
    bus_->BINIT().cycle ();
    cpuControl_->start (address);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_P, address);
}

// Process the given character in the state machine, returning true if we can
// accept another character, i.e. the state machine is still running.
bool KD11_NA_ODT::processCharacter (u8 character)
{
    stateMachine_->dispatch (createEvent (echoCharacter (character)));
    return odtRunning_;
}
