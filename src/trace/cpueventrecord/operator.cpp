#include "cpueventrecord.h"

std::ostream& operator<< (std::ostream& os, TraceRecord<CpuEventRecord> record)
{
    switch (record.type_)
    {
        case CpuEventRecordType::CPU_TRAP:
            os << "[KD11] handling trap: " << oct << record.value_ << '\n';
            break;

        case CpuEventRecordType::CPU_HALT:
        case CpuEventRecordType::CPU_WAIT:
            os << "[KD11] HALT @ " << oct << record.value_ << '\n';
            break;

        case CpuEventRecordType::CPU_RUN:
            os << "[KD11] resume execution at " << oct << record.value_ << '\n';
            break;

        case CpuEventRecordType::CPU_DBLBUS:
            os << "[KD11] double bus error [" << oct << record.value_ << "]\n";
            break;

        default:
            os << "[KD11] CpuEventRecordType :" << static_cast<int> (record.type_) << '\n';
            break;
    }
    return os;
}