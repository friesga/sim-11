#include "kdf11_odt.h"

// Create the appropriate event from the given character. We receive either
// a character or an empty object. In the latter case we create an
// ExitCmdGiven event to make the state machine transition to the exit point.
// 
// Where applicable, upper- and lower case of command characters are
// recognized. (EK-KDF11-UG-PR2 p. 3-6)
// 
// To prevent accidental toggling of the halt flip-flop, the H that is typed
// must be uppercase (shift H). All other OOT commands may be typed in either
// upper or lowercase. (EK-11024-TM-003 p. 3-9)
//
KDF11_ODT::Event KDF11_ODT::createEvent (CondData<u8> c)
{
    if (!c.hasValue ())
        return ExitCmdGiven {};

    switch (c)
    {
        case '\r':
            return CloseLocationCmdEntered {};

        case '\n':
            return OpenNextLocationCmdEntered {};

        case DC3:
            return BinaryDumpCmdEntered {};

        case '/':
            return OpenLocationCmdEntered {};

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            return DigitEntered {c};

        case 'g':
        case 'G':
            // Go command
            return GoCmdEntered {};

        case 'p':
        case 'P':
            // Proceed command
            return ProceedCmdEntered {};

        case 'H':
            // Halt command
            return HaltCmdEntered {};

        case 'r':
        case 'R':
        case '$':
            return RegisterCmdEntered {};

        case 's':
        case 'S':
            return PswDesignatorEntered {};

        case ';':
            // The semicolon is accepted for PDP-11 ODT compatibility. If the
            // semicolon character is received during any character sequence,
            // the console ignores it. (Microprocessor Computers Handbook,
            // page 6-7).
            return DiscardCharEntered {};

        default:
            // An invalid character was entered
            return InvalidCharEntered {};
    }
}