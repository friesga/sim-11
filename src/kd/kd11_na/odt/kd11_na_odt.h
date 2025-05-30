#ifndef _KD11_NA_ODT_H_
#define _KD11_NA_ODT_H_

#include "kd/include/kd11odt.h"
#include "bus/include/bus.h"
#include "kd/kd11_na/cpucontrol/kd11_na_cpucontrol.h"
#include "variantfsm/fsm.h"
#include "kd/common/odt/location/location.h"
#include "conddata/conddata.h"
#include "kd/include/consoleaccess.h"
#include "kd/common/odt/operatorconsoleaccess/operatorconsoleaccess.h"
#include "kd/include/cpudata.h"

#include <string>

using std::unique_ptr;
using std::string;
using std::monostate;

class KD11_NA_ODT : public KD11ODT
{
public:
    KD11_NA_ODT (Bus* bus, CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, unique_ptr<ConsoleAccess> consoleAccess);

    // Definition of the function required by the KD11ODT interface
    bool processCharacter (u8 character) override;

    static unique_ptr<KD11_NA_ODT> createODT (Bus *bus, CpuData* cpuData,
        CpuControl* cpuControl, MMU* mmu, unique_ptr<ConsoleAccess> consoleAccess);

private:
    enum { BASE8 = 8 };
    enum { DC3 = 023 };               // ASCII code for Cntrl-shift-s
    enum { BS = 010 };                // ASCII code for delete character
    enum { PswDesignator = 'S' };

    // Definition of the events to be processed by the ODT command parser
    struct StartFsm {};
    struct DigitEntered                     // 0-7
    {
        u8 digit;
    };

    struct InvalidCharEntered {};
    struct DiscardCharEntered {};               // ;
    struct ProceedCmdEntered {};                // P
    struct GoCmdEntered {};                     // G
    struct OpenLocationCmdEntered {};           // /
    struct CloseLocationCmdEntered {};          // <CR>
    struct OpenNextLocationCmdEntered {};       // <LF>
    struct RegisterCmdEntered {};               // R or $
    struct PswDesignatorEntered {};             // S
    struct BinaryDumpCmdEntered {};             // Cntrl-shift-s
    struct ExitCmdGiven {};                     // BA11_N close button clicked

    // The following commands are only implemented in the LSI-11 (KD11-F
    // processor) ODT and are not implemented in the PDP-11/23 (KDF11 processor).
    struct OpenPreviousLocationCmdEntered {};   // Up arrow (ASCII 135)
    struct AtSignCmdEntered {};                 // @
    struct BackArrowCmdEntered {};              // Back arrow (ASCII 137)
    struct RuboutEntered {};                    // Rubout (ASCII 177)
    struct MaintenanceCmdEntered {};            // M

    // OOT has 10 internal states, with each state having its own set of valid
    // input characters. Some commands are allowed only when in certain states
    // or sequences; thus an attempt has been made to lower the probability
    // of a user's unconsciously destroying data by pressing the wrong key
    // (EK-KDJ1A-UG-001).
    // 
    // Table 3-2 in that document defines the OOT states and valid input
    // characters. This implementation of ODT closely follows these states and
    // transitions as defined in that table. 
    // 
    // Definition of the ODT command parser states. These states match the states
    // as defined in table 3-2 in the KDJ11-A CPU Module Users's Guide
    // (EK-KDJ1A-UG-001). The number postfixed to the state name matches the
    // state number in table 3-2. ExitPoint is an additional state to determine
    // the exit of the state machine (on a Go or Proceed command).
    //
    struct EntryPoint {};
    struct AtPrompt_1 {};
    struct StartingRegister_2 {};
    struct AddressOpened_3 {};
    struct RegisterOpened_4 {};
    struct EnteringAddress_5 {};
    struct EnteringRegister_6 {};
    struct EnteringAddressValue_7 {};
    struct EnteringRegisterValue_8 {};
    struct EnteringBinaryDump_10 {};
    struct ExitPoint {};

    // Definition of the events and states to use. The first state member of the
    // variant is the initial state.
    using Event = std::variant<StartFsm,
        DigitEntered,
        InvalidCharEntered,
        DiscardCharEntered,
        ProceedCmdEntered,
        GoCmdEntered,
        OpenLocationCmdEntered,
        CloseLocationCmdEntered,
        OpenNextLocationCmdEntered,
        RegisterCmdEntered,
        PswDesignatorEntered,
        BinaryDumpCmdEntered,
        ExitCmdGiven,
        OpenPreviousLocationCmdEntered,
        AtSignCmdEntered,
        BackArrowCmdEntered,
        RuboutEntered,
        MaintenanceCmdEntered>;

    using State = std::variant<EntryPoint,
        AtPrompt_1,
        StartingRegister_2,
        AddressOpened_3,
        RegisterOpened_4,
        EnteringAddress_5,
        EnteringRegister_6,
        EnteringAddressValue_7,
        EnteringRegisterValue_8,
        EnteringBinaryDump_10,
        ExitPoint,
        monostate>;

    // Use the PIMPL idiom to be able to define the StateMachine outside
    // of the Switch class
    class StateMachine;
    unique_ptr<StateMachine> stateMachine_;

    Bus* bus_;
    CpuData* cpuData_;
    CpuControl* cpuControl_;
    MMU* mmu_;
    unique_ptr<ConsoleAccess> console_;
    bool odtRunning_;
    string digitSeries_;
    u16 newValue_;
    string registerSeries_;
    Location<u16> location_;

    CondData<u8> echoCharacter (CondData<u8> c);
    void writeString (string str);
    State writeAddressContents (u16 address);
    string octalNumberToString (u16 number);
    u32 stringToOctalNumber (string str);
    bool stringTou16 (string str, size_t nDigits, u16* value);
    Event createEvent (CondData<u8> c);
    State openAddress ();
    void setAddressValue ();
    State openNextAddress (std::function<u16 (void)> getNextRegister);
    State openNextRegister (State&& currentState,
        std::function<u8 (void)> getNextRegister);
    bool registerSeriesEndsWith (string);
    void setRegisterValue ();
    void startCPU (u16 address);
};

class KD11_NA_ODT::StateMachine : public variantFsm::Fsm<StateMachine, Event, State>
{
public:
    StateMachine (KD11_NA_ODT* context);
    // Definition of entry actions and state transitions for valid characters.
    // Invalid characters cause the default transition defined below to occur.
    // 
    // The actions and transitions are defined public as they are called by
    // several (templated) functions in the state machine.
    State transition (EntryPoint&&, StartFsm);

    void entry (AtPrompt_1);
    State transition (AtPrompt_1&&, DigitEntered);                                 // -> EnteringAddress_5
    State transition (AtPrompt_1&&, RegisterCmdEntered);                           // -> StartingRegister_2
    State transition (AtPrompt_1&&, OpenLocationCmdEntered);                       // -> AddressOpenend__3/RegisterOpened_4
    State transition (AtPrompt_1&&, ProceedCmdEntered);                            // -> ExitPoint
    State transition (AtPrompt_1&&, GoCmdEntered);                                 // -> ExitPoint
    State transition (AtPrompt_1&&, BinaryDumpCmdEntered);                         // -> EnteringBinaryDump_10
    State transition (AtPrompt_1&&, MaintenanceCmdEntered);                        // -> AtPrompt_1

    State transition (EnteringAddress_5&&, DigitEntered);                          // -> EnteringAddress_5
    State transition (EnteringAddress_5&&, RuboutEntered);                         // -> EnteringAddress_5
    State transition (EnteringAddress_5&&, OpenLocationCmdEntered);                // -> AddressOpened_3
    State transition (EnteringAddress_5&&, GoCmdEntered);                          // -> ExitPoint
    State transition (EnteringAddress_5&&, CloseLocationCmdEntered);               // -> AtPrompt_1

    State transition (AddressOpened_3&&, DigitEntered);                            // -> EnteringAddressValue_7
    State transition (AddressOpened_3&&, RuboutEntered);                           // -> EnteringAddressValue_7
    State transition (AddressOpened_3&&, CloseLocationCmdEntered);                 // -> AtPrompt_1
    State transition (AddressOpened_3&&, OpenNextLocationCmdEntered);              // -> AddressOpened_3
    State transition (AddressOpened_3&&, OpenPreviousLocationCmdEntered);          // -> AddressOpened_3
    State transition (AddressOpened_3&&, AtSignCmdEntered);                        // -> AddressOpened_3
    State transition (AddressOpened_3&&, BackArrowCmdEntered);                     // -> AddressOpened_3
    State transition (AddressOpened_3&&, RegisterCmdEntered);                      // -> StartingRegister_2

    State transition (EnteringAddressValue_7&&, DigitEntered);                     // -> EnteringAddressValue_7
    State transition (EnteringAddressValue_7&&, RuboutEntered);                    // -> EnteringAddressValue_7
    State transition (EnteringAddressValue_7&&, CloseLocationCmdEntered);          // -> AtPrompt_1
    State transition (EnteringAddressValue_7&&, OpenNextLocationCmdEntered);       // -> AddressOpened_3
    State transition (EnteringAddressValue_7&&, OpenPreviousLocationCmdEntered);   // -> AddressOpened_3
    State transition (EnteringAddressValue_7&&, AtSignCmdEntered);                 // -> AddressOpened_3
    State transition (EnteringAddressValue_7&&, BackArrowCmdEntered);              // -> AddressOpened_3
    State transition (EnteringAddressValue_7&&, OpenLocationCmdEntered);           // -> AddressOpened_3

    State transition (StartingRegister_2&&, DigitEntered);                         // -> EnteringRegister_6
    State transition (StartingRegister_2&&, RuboutEntered);                        // -> EnteringAddress_5
    State transition (StartingRegister_2&&, PswDesignatorEntered);                 // -> EnteringRegister_6

    State transition (EnteringRegister_6&&, DigitEntered);                         // -> EnteringRegister_6
    State transition (EnteringRegister_6&&, RuboutEntered);                        // -> EnteringAddress_5
    State transition (EnteringRegister_6&&, PswDesignatorEntered);                 // -> EnteringRegister_6
    State transition (EnteringRegister_6&&, OpenLocationCmdEntered);               // -> RegisterOpenend_4
    State transition (EnteringRegister_6&&, CloseLocationCmdEntered);              // -> AtPrompt_1

    State transition (RegisterOpened_4&&, CloseLocationCmdEntered);                // -> AtPrompt_1
    State transition (RegisterOpened_4&&, DigitEntered);                           // -> EnteringRegisterValue_8
    State transition (RegisterOpened_4&&, RuboutEntered);                          // -> RegisterOpened_4
    State transition (RegisterOpened_4&&, OpenNextLocationCmdEntered);             // -> RegisterOpened_4/AtPrompt_1
    State transition (RegisterOpened_4&&, OpenPreviousLocationCmdEntered);         // -> RegisterOpened_4/AtPrompt_1
    State transition (RegisterOpened_4&&, AtSignCmdEntered);                       // -> AddressOpened_3
    State transition (RegisterOpened_4&&, BackArrowCmdEntered);                    // -> AtPrompt_1
    State transition (RegisterOpened_4&&, RegisterCmdEntered);                     // -> StartingRegister_2

    State transition (EnteringRegisterValue_8&&, DigitEntered);                    // -> EnteringRegisterValue_8
    State transition (EnteringRegisterValue_8&&, RuboutEntered);                   // -> EnteringRegisterValue_8
    State transition (EnteringRegisterValue_8&&, CloseLocationCmdEntered);         // -> AtPrompt_1
    State transition (EnteringRegisterValue_8&&, OpenLocationCmdEntered);          // -> AddressOpened_3
    State transition (EnteringRegisterValue_8&&, OpenNextLocationCmdEntered);      // -> EnteringRegisterValue_8/AtPrompt_1
    State transition (EnteringRegisterValue_8&&, OpenPreviousLocationCmdEntered);  // -> EnteringRegisterValue_8/AtPrompt_1
    State transition (EnteringRegisterValue_8&&, AtSignCmdEntered);                // -> AddressOpened_3
    State transition (EnteringRegisterValue_8&&, BackArrowCmdEntered);             // -> AtPrompt_1

    void entry (ExitPoint);

    // Define a transition from all states to the ExitPoint state on an
    // ExitCmdGiven event
    template <typename S>
    State transition (S&&, ExitCmdGiven)
    {
        return ExitPoint {};
    }

    // Define a transition to the same state on characters to be
    // discarded. The transition is defined as an internal transition
    // to prevent exit and entry actions to be executed.
    template <typename S>
    State transition (S&&, DiscardCharEntered)
    {
        return monostate {};
    }

    // Define the default transition for transitions not explicitly
    // defined above. The default transition implies a question mark is
    // printed and the AtPrompt_1 state is entered.
    template <typename S, typename E>
    State transition (S&& state, E)
    {
        context_->writeString ("?\n");
        return AtPrompt_1 {};
    }

    // As we make use of exit/entry functions, we must handle all cases.
    // The default entry/exit action is an immediate return.
    template <typename S> void exit (variantFsm::TagType<S>) {}
    template <typename S> void entry (S&) {}

private:
    KD11_NA_ODT* context_;
};

inline KD11_NA_ODT::StateMachine::StateMachine (KD11_NA_ODT* context)
    :
    context_ {context}
{}

#endif // _KD11_NA_ODT_H_