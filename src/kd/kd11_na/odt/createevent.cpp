#include "kd11_na_odt.h"

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
KD11_NA_ODT::Event KD11_NA_ODT::createEvent (CondData<u8> c)
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

        case '@':
            return AtSignCmdEntered {};

        case 'G':
            // Go command
            return GoCmdEntered {};

        case 'M':
            // Maintenance command
            return MaintenanceCmdEntered {};

        case 'P':
            // Proceed command
            return ProceedCmdEntered {};

        case 'R':
        case '$':
            return RegisterCmdEntered {};

        case 'S':
            return PswDesignatorEntered {};

        case '^':
            // ASCII 136 up arrow
            // Descriptions of the Up Arrow command in the "Microprocessor
            // Computers" and "Microcomputer and Memories" handbooks define
            // ASCII character 0135 as the up arrow command. Tests executed
            // on a LSI-11/2 show that this is a mistake and that the actual
            // up arrow command is ASCII character 0136 (^).
            return OpenPreviousLocationCmdEntered {};

        case '_':
            // ASCII 137 back arrow
            return BackArrowCmdEntered {};

        case BS:
            // On VT100 compatible terminals, this is the character produced
            // by the key marked "<=". On modern machines it is called
            // backspace and doesn't match the PC delete key.
            // (https://www.asciihex.com/character/control/127/0x7F/del-delete-character)
            return RuboutEntered {};

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