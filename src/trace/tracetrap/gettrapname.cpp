#include "tracetrap.h"

// Return the trap name for the given trap vector.
//
// String literals have static storage duration, and thus exist in memory
// for the life of the program. The pointer returned by this function remains
// valid in the life of the program.
const char* TraceRecord<TraceTrap>::getTrapName (int vector)
{
    switch (vector)
    {
        case 0004: /* Bus error, Time out */
            return "Bus error / time out";
            break;

        case 0010: /* Reserved */
            return "Reserved instruction";

        case 0014: /* BPT trap instruction, T bit */
            return "BPT trap instruction / T bit";

        case 0020: /* IOT executed */
            return "IOT executed";

        case 0024: /* Power fail / restart */
            return "Power fail / restart";

        case 0030: /* EMT executed */
            return "EMT executed";

        case 0034: /* TRAP executed */
            return "TRAP executed";

        case 0060: /* Console input device */
            return "Console input device";

        case 0064: /* Console output device */
            return "Console output device";

        case 0100: /* External event line interrupt */
            return "External event line interrupt";

        case 0244: /* FIS trap */
            return "FIS trap";

        default:
            return NULL;
    }
}