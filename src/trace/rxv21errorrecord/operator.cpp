#include "rxv21errorrecord.h"

#include <iomanip>

using std::oct;
using std::setw;
using std::setfill;

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os, TraceRecord<RXV21Error> record)
{
    switch (record.type_)
    {
        case RXV21ErrorType::RXV21_WC_OVFL:
        case RXV21ErrorType::RXV21_TRACK_NO:
        case RXV21ErrorType::RXV21_SECT_NO:
            // fprintf (DST, "[RXV21] Error: %s [%o/%d]\n",
            //    name, info, info);
            os << "[RXV21] Error: " << record.getErrorName () <<
            " [" << oct << setw (6) << setfill('0') << record.info_ << "]\n";
            break;

        case RXV21ErrorType::RXV21_DEN_ERR:
        default:
            // fprintf (DST, "[RXV21] Error: %s\n", name);
            os << "[RXV21] Error: " << record.getErrorName () << '\n';
            break;
    }
    return os;
}