#include "ms11precord.h"

using std::oct;
using std::dec;

std::ostream& operator<< (std::ostream& os, TraceRecord<MS11_PRecord> record)
{
    switch (record.type_)
    {
        case MS11_PRecordType::ReadCSR:
            os << "[MS1P] read CSR: " << oct << record.csr_ << '\n';
            break;

        case MS11_PRecordType::WriteCSR:
            os << "[MS1P] write CSR: " << oct << record.csr_ << '\n';
            break;

        case MS11_PRecordType::ReadMemory:
            os << "[MS1P] read memory " << oct << record.address_ <<
                ": " << record.value_ << '/' << 
                oct << +record.checkBits_ << 
                " CSR: " << oct << record.csr_ << '\n';
            break;

        case MS11_PRecordType::WriteMemory:
            os << "[MS1P] write memory " << oct << record.address_ <<
                ": " << record.value_ << '/' <<
                oct << +record.checkBits_ << 
                " CSR: " << oct << record.csr_ << '\n';
            break;
    }
    return os;
}