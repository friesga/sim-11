#include "busrecord.h"

std::ostream& operator<< (std::ostream& os, TraceRecord<BusRecord> record)
{
    switch (record.type_)
    {
        case BusRecordType::Read:
            os << std::oct << "[Qbus] read " << 
                record.address_ << " = " << record.value_ << '\n';
            break;

        case BusRecordType::Write:
            os << std::oct << "[Qbus] write " << 
                record.address_ << " =  " << record.value_ << '\n';
            break;

        case BusRecordType::ReadFail:
            os << std::oct << "[Qbus] read " << record.address_ << " timed out\n";
            break;

        case BusRecordType::WriteFail:
            os << std::oct << "[Qbus] write " 
                << record.address_ << " = " << record.value_ << "timed out\n";
            break;

        case BusRecordType::BusReset:
            os << std::oct << "[Qbus] reset\n";
            break;
    }
    return os;
}