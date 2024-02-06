#include "kdf11_odt.h"
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

KDF11_ODT::KDF11_ODT (Qbus *bus, CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, unique_ptr<ConsoleAccess> consoleAccess)
    : 
    bus_ {bus},
    cpuData_ {cpuData},
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

unique_ptr<KDF11_ODT> KDF11_ODT::createODT (Qbus *bus, CpuData* cpuData,
    CpuControl* cpuControl, MMU* mmu, unique_ptr<ConsoleAccess> consoleAccess)
{
    return move (make_unique<KDF11_ODT> (bus, cpuData, cpuControl, mmu,
        make_unique<OperatorConsoleAccess> (bus)));
}

CondData<u8> KDF11_ODT::echoCharacter (CondData<u8> c)
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
void KDF11_ODT::writeString (string str)
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
KDF11_ODT::State KDF11_ODT::writeAddressContents (u32 address)
{
    location_ = AddressLocation<u32> {address};
    if (bus_->read (BusAddress (address, BusAddress::Width::_18Bit)).hasValue ())
    {
        writeString (octalNumberToString (bus_->read (BusAddress (location_.wordAddress (),
            BusAddress::Width::_18Bit))) + ' ');
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
string KDF11_ODT::octalNumberToString (u32 number)
{
    stringstream tmp {};
    tmp << oct << setfill ('0') << setw (6) << number;
    return tmp.str ();
}

u32 KDF11_ODT::stringToOctalNumber (string str)
{
    stringstream tmp {str};
    u32 returnValue;
    tmp >> oct >> returnValue;
    return returnValue;
}

// Convert the least significant 16 bits from the given octal number
// string to a u16 and report the success or failure of this conversion.
bool KDF11_ODT::stringTou16 (string str, size_t nDigits, u16 *value)
{
    if (str.empty ())
        return false;

    u32 tmp = stringToOctalNumber (str);
    *value = static_cast<u16> (tmp & 0177777);
    return true;
}

// Convert the least significant 18 bits from the given octal number
// string to a u32 and report the success or failure of this conversion.
bool KDF11_ODT::stringTou18 (string str, size_t nDigits, u32 *value)
{
    if (str.empty ())
        return false;

    *value = stringToOctalNumber (str) & 0777777;
    return true;
}

bool KDF11_ODT::registerSeriesEndsWith (string str)
{
    if (registerSeries_.size() >= 3)
    {
        return registerSeries_.substr (registerSeries_.size() - 3) == str;
    }
    else
        return false;
}

KDF11_ODT::State KDF11_ODT::openAddress ()
{
    // Convert the last eight digits entered to an address. This can lead
    // to an address larger than the available memory.
    u32 address {0};
    stringTou18 (digitSeries_, 8, &address);
    
    // The specified address might be a byte or a word address. This is
    // dealt with in the Location class.
    return writeAddressContents (address);
}

// Convert the least significant 16 bits of the value entered to the new
// value. If this value exceeds the maximal value of a 16 bit word display an error.
void KDF11_ODT::setAddressValue ()
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
// Incrementing <LF>, the addresses 177776, 377776, 577776 and 777776 result
// in the addresses 000000, 200000, 400000, and 600000, respectively. That is,
// the upper 2 bits of the 18-bit address are not affected; they must be
// explicitly set. (EK-KDFEB-UG)
// 
KDF11_ODT::State KDF11_ODT::openNextAddress ()
{
    u32 address = (location_.inputAddress () & 0600000) | 
        ((location_.inputAddress () + 2) % 0200000);
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
KDF11_ODT::State KDF11_ODT::openNextRegister (State &&currentState, 
    std::function<u8(void)> getNextRegister)
{
    if (location_.isA<PSWLocation> ())
        return AtPrompt_1 {};

    u8 registerNr = getNextRegister ();
    location_ = RegisterLocation {registerNr};
    writeString ("R" + to_string(registerNr) + '/' + 
        octalNumberToString (cpuData_->registers ()[registerNr]) + ' ');
    return move (currentState);
}

// Note that the contents of the PSW can be changed using the CR
// command but bit 4 (the T bit) cannot be set using any of the
// commands. (LSI11 PDP11/03 Processor Handbook)
void KDF11_ODT::setRegisterValue ()
{
    // Convert the last six digits entered to the new value. If this value
    // exceeds the maximal value of a 16 bit word display an error.
    if (stringTou16 (digitSeries_, 6, &newValue_))
    {
        // Either a register or the PSW has to be set to the new value.
        if (location_.isA<RegisterLocation> ())
            cpuData_->registers ()[location_.registerNr ()] = newValue_;
        else
            // Setting or clearing the PSW T-bit will be prohibited
            // by PSW::set().
            cpuData_->psw ().set (PSW::ProtectionMode::ExplicitAccess,
                newValue_);
    }
    else
        writeString ("?\n");
}

// Set the CPU into the running state with the specified address as the PC
// 
// On a hardware KDF11-A an 18-bit address has to be specified to start the
// CPU. This makes little sense however as the PC is 16-bits and the CPU is
// running in umapped mode, so also using 16-bit addresses. In the KDF11-A
// the 18-bit address is used to address the I/O page but that should not
// be necessary when we're using 16-bit addresses with proper mapping to the
// I/O page.
void KDF11_ODT::startCPU (u16 address)
{
    bus_->BINIT().cycle ();
    mmu_->reset ();
    cpuControl_->start (address);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_P, address);
}

// Process the given character in the state machine, returning true if we can
// accept another character, i.e. the state machine is still running.
bool KDF11_ODT::processCharacter (u8 character)
{
    stateMachine_->dispatch (createEvent (echoCharacter (character)));
    return odtRunning_;
}

// Test if the given string ends with the second given string
bool KDF11_ODT::endsWith (string const &completeString, string const &endString)
{
    if (completeString.length() >= endString.length())
        return (completeString.compare (completeString.length() - endString.length(),
            endString.length(), endString) == 0);
    else
        return false;
}
