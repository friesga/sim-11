#include "rlv12registersrecord.h"

#include <iomanip>

using std::oct;
using std::setfill;
using std::right;
using std::setw;

// Overloaded operator<< function to print RLV12Registers records
std::ostream& operator<< (std::ostream& os, TraceRecord<RLV12RegistersRecord> record)
{
    os << "[RLV12] " << record.msg_ << ": " << 
        "RLCS: "  << oct << setw (6) << setfill ('0') << record.rlcs_  << ", " <<
        "RLBA: "  << oct << setw (6) << setfill ('0') << record.rlba_  << ", " <<
        "RLDA: "  << oct << setw (6) << setfill ('0') << record.rlda_  << ", " <<
        "RLMPR: " << oct << setw (6) << setfill ('0') << record.rlmpr_ << ", " <<
        "RLBAE: " << oct << setw (6) << setfill ('0') << record.rlbae_ << '\n';

    return os;
}