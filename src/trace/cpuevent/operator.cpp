#include "cpuevent.h"

std::ostream& operator<< (std::ostream& os, TraceRecord<CpuEvent> record)
{
    switch (record.type_)
    {
        case CpuEventType::CPU_TRAP:
            os << "[KD11] handling trap: " << oct << record.value_ << '\n';
            break;

        case CpuEventType::CPU_HALT:
        case CpuEventType::CPU_WAIT:
            os << "[KD11] HALT @ " << oct << record.value_ << '\n';
            break;

        case CpuEventType::CPU_RUN:
            os << "[KD11] resume execution at " << oct << record.value_ << '\n';
            break;

        case CpuEventType::CPU_DBLBUS:
            os << "[KD11] double bus error [" << oct << record.value_ << "]\n";
            break;

        default:
            os << "[KD11] CpuEventType :" << static_cast<int> (record.type_) << '\n';
            break;
    }
    return os;
}