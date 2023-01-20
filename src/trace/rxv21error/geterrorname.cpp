#include "rxv21error.h"

const char* TraceRecord<RXV21Error>::getErrorName ()
{
    switch (type_)
    {
        case RXV21ErrorType::RXV21_WC_OVFL:
            return "Word Count overflow";

        case RXV21ErrorType::RXV21_DEN_ERR:
            return "Density mismatch";

        case RXV21ErrorType::RXV21_TRACK_NO:
            return "Tried to access a track greater than 76";

        case RXV21ErrorType::RXV21_SECT_NO:
            return "Desired sector could not be found after looking at 52 headers (2 revolutions)";

        default:
            return "unknown";
    }
}
