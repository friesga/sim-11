#include "cpueventrecord.h"

std::ostream& operator<< (std::ostream& os, TraceRecord<CpuEventRecord> record)
{
    switch (record.type_)
    {
        case CpuEventRecordType::CPU_TRAP:
            os << "[KD11] handling trap: " << oct << record.value_ << '\n';
            break;

        case CpuEventRecordType::CPU_HALT:
            os << "[KD11] CPU_HALT @ " << oct << record.value_ << '\n';
            break;

        case CpuEventRecordType::CPU_WAIT:
            os << "[KD11] CPU_WAIT @ " << oct << record.value_ << '\n';
            break;

        case CpuEventRecordType::CPU_RUN:
            os << "[KD11] resume execution at " << oct << record.value_ << '\n';
            break;

        case CpuEventRecordType::CPU_DBLBUS:
            os << "[KD11] double bus error [" << oct << record.value_ << "]\n";
            break;

        case CpuEventRecordType::CPU_ODT_P:
            os << "[KD11] ODT Proceed " << oct << record.value_ << '\n';
            break;

        case CpuEventRecordType::CPU_ODT_G:
            os << "[KD11] ODT Go " << oct << record.value_ << '\n';
            break;

        default:
            os << "[KD11] CpuEventRecordType :" << static_cast<int> (record.type_) << '\n';
            break;
    }
    return os;
}