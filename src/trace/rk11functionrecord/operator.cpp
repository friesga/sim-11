#include "rk11functionrecord.h"

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os,
    TraceRecord<RK11FunctionRecord> record)
{
    os << "[RK11] Function: " << record.getFunctionName () <<
        " DA: " << record.function_.diskAddress.value <<
        " WC: " << record.function_.wordCount <<
        " BA: " << record.function_.busAddress << '\n';
    return os;
}