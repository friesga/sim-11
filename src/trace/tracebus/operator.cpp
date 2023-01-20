#include "tracebus.h"

std::ostream& operator<< (std::ostream& os, TraceRecord<TraceBus> record)
{
    switch (record.type_)
    {
        case TraceBusType::Read:
            os << std::oct << "[QBUS] read " << 
                record.address_ << " = " << record.value_ << '\n';
            break;

        case TraceBusType::Write:
            os << std::oct << "[QBUS] write " << 
                record.address_ << " =  " << record.value_ << '\n';
            break;

        case TraceBusType::ReadFail:
            os << std::oct << "[QBUS] read " << record.address_ << " timed out\n";
            break;

        case TraceBusType::WriteFail:
            os << std::oct << "[QBUS] write " 
                << record.address_ << " = " << record.value_ << "timed out\n";
            break;

        case TraceBusType::BusReset:
            os << std::oct << "[QBUS] reset\n";
            break;
    }
    return os;
}