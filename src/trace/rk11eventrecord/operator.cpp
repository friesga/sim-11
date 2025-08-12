#include "rk11eventrecord.h"
#include "trace/rk11names/rk11names.h"

#include <iomanip>
#include <variant>

using std::setw;
using std::setfill;
using std::oct;
using std::dec;
using std::holds_alternative;

using RK11Names::getFunctionName;
using RK11Names::getStateName;

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os,
    TraceRecord<RK11EventRecord> record)
{
    os << "[RK11] State [" << getStateName (record.state_) << "] ";

    if (holds_alternative<RKTypes::SeekCompleteReport> (record.event_))
    {
        RKTypes::SeekCompleteReport report = 
            std::get<RKTypes::SeekCompleteReport>(record.event_);
        os << "SeekComplete [driveId=" << report.driveId << ", " <<
            "RKER=" << setw (7) << oct << report.rker.value << "]\n";
    }
    else if (holds_alternative<RKTypes::Function> (record.event_))
    {
        RKTypes::Function function = 
            std::get<RKTypes::Function>(record.event_);
        os << "Function [" << getFunctionName (function) <<
            " (" << function.rkcs.operation << ')' <<
            " RKCS=" << setw (7) << oct << function.rkcs.value <<
            " RKDA=" << setw (7) << oct << function.diskAddress.value <<
            " RKWC=" << function.wordCount <<
            " RKBA=" << setw (7) << oct << function.busAddress << "]\n";
    }
    else
        throw std::runtime_error ("Unknown event type");
    
    return os;
}