#include "traceirq.h"

#include <iomanip>

using std::oct;
using std::setw;
using std::setfill;

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os, TraceRecord<TraceIrq> record)
{
    switch (record.type_)
    {
        case TraceIrqType::IRQ_OK:
            // fprintf (DST, "[QBUS] interrupt request %o\n", n);
            os << "[QBUS] interrupt request " << oct << 
                setw(4) << setfill('0') << record.vector_ << '\n';
            break;

        case TraceIrqType::IRQ_FAIL:
            // fprintf (DST, "[QBUS] interrupt request %o denied\n", n);
            os << "[QBUS] interrupt request " << oct << 
                setw(4) << setfill('0') << record.vector_ << "denied\n";
            break;

        case TraceIrqType::IRQ_SIG:
            // fprintf (DST, "[QBUS] signaling irq %o\n", n);
            os << "[QBUS] signaling irq " << oct << 
                setw(4) << setfill('0') << record.vector_ << " denied\n";
            break;
    }

    return os;
}