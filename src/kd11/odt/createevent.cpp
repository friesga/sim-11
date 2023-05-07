#include "kd11odt.h"

using namespace KD11_ODT;

// Create the appropriate event from the given character. We receive either
// a character or an empty object. In the latter case we create an
// ExitCmdGiven event to make the state machine transition to the exit point.
// 
// "All ODT commands must be typed with the CAPS LOCK key depressed. Lower
// case letters are not legal for ODT commands". (EK-11V03-TM-002)
// This contradicts the following statement in the MicroComputer and Memories
// handbook, chapter 7: "Console ODT will recognize uppercase and lowercase
// characters as commands". We presume the latter statement is accurate for
// a KD11-F processor.
Event KD11ODT::createEvent (CondData<u8> c)
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
        case 'g':
            // Go command
            return GoCmdEntered {};

        case 'M':
        case 'm':
            // Maintenance command
            return MaintenanceCmdEntered {};

        case 'P':
        case 'p':
            // Proceed command
            return ProceedCmdEntered {};

        case 'R':
        case 'r':
        case '$':
            return RegisterCmdEntered {};

        case 'S':
        case 's':
            return PswDesignatorEntered {};

        case ']':
            // ASCII 135 up arrow
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

        default:
            // An invalid character was entered
            return InvalidCharEntered {};
    }
}