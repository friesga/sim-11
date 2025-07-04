#include "rk11registersrecord.h"

#include <iomanip>

using std::setw;
using std::setfill;
using std::oct;
using std::dec;

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os,
    TraceRecord<RK11RegistersRecord> record)
{
    os << "[RK11] BA=" << setw (7) << setfill ('0') << oct << record.busAddress_ <<
        " [RKDS=" << setw (7) << setfill ('0') << oct << record.rkds_.value <<
        " RKER="  << setw (7) << setfill ('0') << oct << record.rker_.value <<
        " RKCS="  << setw (7) << setfill ('0') << oct  << record.rkcs_.value <<
        " RKWC="  << dec << record.rkwc_ << 
        " RKBA=" << setw (7) << setfill ('0') << oct << record.rkba_ <<
        " RKDA=" << setw (7) << setfill ('0') << oct << record.rkda_.value <<
        " RKDB=" << setw (7) << setfill ('0') << oct << record.rkdb_ << "]\n";

    return os;
}