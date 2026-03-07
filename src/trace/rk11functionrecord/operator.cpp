#include "rk11functionrecord.h"
#include "trace/rk11names/rk11names.h"

#include <iomanip>

using std::setw;
using std::oct;

using RK11Names::getFunctionName;
using RK11Names::getStateName;

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os,
    TraceRecord<RK11FunctionRecord> record)
{
    os << "[RK11] Function: " << getFunctionName (record.function_) <<
        " (" << record.function_.rkcs.operation << ')' <<
          " DA: " << setw (7) << oct << record.function_.diskAddress.value;
    os << " WC: " << record.function_.wordCount <<
          " BA: " << setw (7) << oct << record.function_.busAddress << '\n';
    return os;
}