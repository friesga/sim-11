#include "irqrecord.h"

#include <iomanip>

using std::oct;
using std::setw;
using std::setfill;

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os, TraceRecord<IrqRecord> record)
{
    switch (record.type_)
    {
        case IrqRecordType::IRQ_REQUEST:
            os << "[IRQ ] Interrupt request [id=" << record.intrpReqId_ <<
                " vector=" << oct << setw(4) << setfill('0') << 
                record.vector_ << "]\n";
            break;

        case IrqRecordType::IRQ_GRANT:
            os << "[IRQ ] Interrupt grant [id=" << record.intrpReqId_ << 
                " vector=" << oct << setw (4) << setfill ('0') <<
                record.vector_ << "]\n";
            break;

        case IrqRecordType::IRQ_CLEAR:
            os << "[IRQ ] Clear interrupt [id=" << record.intrpReqId_ <<
                " vector=" << oct << setw (4) << setfill ('0') <<
                record.vector_ << "]\n";
            break;
    }

    return os;
}