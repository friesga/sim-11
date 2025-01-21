#include "busrecord.h"

using std::oct;
using std::dec;

std::ostream& operator<< (std::ostream& os, TraceRecord<BusRecord> record)
{
    switch (record.type_)
    {
        case BusRecordType::Read:
            os << "[Bus ] read " << oct << record.address_ <<
                " (" << dec << record.width_ << "bit)" <<
                " = " << oct << record.value_ << '\n';
            break;

        case BusRecordType::Write:
            os << "[Bus ] write " << oct << record.address_ <<
                " (" << dec<< record.width_ << "bit)" <<
                " = " << oct << record.value_ << '\n';
            break;

        case BusRecordType::ReadFail:
            os << "[Bus ] read " << oct << record.address_ <<
                " (" << dec << record.width_ << "bit)" << " timed out\n";
            break;

        case BusRecordType::WriteFail:
            os << "[Bus ] write " << oct << record.address_ <<
                " (" << dec << record.width_ << "bit)" <<
                " = " << oct << record.value_ << "timed out\n";
            break;

        case BusRecordType::BusReset:
            os << std::oct << "[Bus ] reset\n";
            break;
    }
    return os;
}