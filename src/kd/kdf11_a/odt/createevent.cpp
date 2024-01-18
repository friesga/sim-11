#include "kdf11_a_odt.h"

// Create the appropriate event from the given character. We receive either
// a character or an empty object. In the latter case we create an
// ExitCmdGiven event to make the state machine transition to the exit point.
// 
// "All ODT commands must be typed with the CAPS LOCK key depressed. Lower
// case letters are not legal for ODT commands". (EK-11V03-TM-002)
// This contradicts the following statement in the MicroComputer and Memories
// handbook, chapter 7: "Console ODT will recognize uppercase and lowercase
// characters as commands". The test runs conducted at a real LSI-11/2
// demonstrated that lower case characters are not accepted.
//
KDF11_A_ODT::Event KDF11_A_ODT::createEvent (CondData<u8> c)
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

        case 'G':
            // Go command
            return GoCmdEntered {};

        case 'P':
            // Proceed command
            return ProceedCmdEntered {};

        case 'R':
        case '$':
            return RegisterCmdEntered {};

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