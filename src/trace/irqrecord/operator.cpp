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
        case IrqRecordType::IRQ_OK:
            // fprintf (DST, "[Qbus] interrupt request %o\n", n);
            os << "[Qbus] interrupt request " << oct << 
                setw(4) << setfill('0') << record.vector_ << '\n';
            break;

        case IrqRecordType::IRQ_FAIL:
            // fprintf (DST, "[Qbus] interrupt request %o denied\n", n);
            os << "[Qbus] interrupt request " << oct << 
                setw(4) << setfill('0') << record.vector_ << "denied\n";
            break;

        case IrqRecordType::IRQ_SIG:
            // fprintf (DST, "[Qbus] signaling irq %o\n", n);
            os << "[Qbus] signaling irq " << oct << 
                setw(4) << setfill('0') << record.vector_ << " denied\n";
            break;
    }

    return os;
}