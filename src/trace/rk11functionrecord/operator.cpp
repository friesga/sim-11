#include "rk11functionrecord.h"

#include <iomanip>

using std::setw;
using std::oct;

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os,
    TraceRecord<RK11FunctionRecord> record)
{
    os << "[RK11] Function: " << record.getFunctionName () <<
        " (" << record.function_.operation << ')' <<
          " DA: " << setw (7) << oct << record.function_.diskAddress.value;
    os << " WC: " << record.function_.wordCount <<
          " BA: " << setw (7) << oct << record.function_.busAddress << '\n';
    return os;
}