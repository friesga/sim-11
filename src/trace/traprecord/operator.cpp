#include "traprecord.h"

#include <iomanip>

using std::oct;
using std::setw;
using std::setfill;

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os, TraceRecord<TrapRecord> record)
{
    const char* name;
    switch (record.cause_)
    {
        case TrapRecordType::TRAP:
            name = record.getTrapName (record.vector_);
            if (name)
                // fprintf (DST, "[KD11_NA] TRAP %o: %s\n", n, name);
                os << "[KD11_NA] TRAP " << setw (4) << setfill('0') <<
                    record.vector_ << ": " << name << '\n';
            else
                // fprintf (DST, "[KD11_NA] TRAP %o\n", n);
                os << "[KD11_NA] TRAP " << setw (4) << setfill('0') <<
                    record.vector_ << '\n';
            break;

        case TrapRecordType::TRAP_T:
            // fprintf (DST, "[KD11_NA] TRAP %o: T bit\n", n);
            os << "[KD11_NA] TRAP " << setw (4) << setfill('0') <<
                    record.vector_ << " T bit\n";
            break;

        case TrapRecordType::TRAP_RADDR:
            // fprintf (DST, "[KD11_NA] TRAP %o: get address on mode 0\n", n);
            os << "[KD11_NA] TRAP " << setw (4) << setfill('0') <<
                    record.vector_ << ": get address on mode 0\n";
            break;

        case TrapRecordType::TRAP_ILL:
            // fprintf (DST, "[KD11_NA] TRAP %o: illegal instruction\n", n);
            os << "[KD11_NA] TRAP " << setw (4) << setfill('0') <<
                    record.vector_ << ": illegal instruction\n";
            break;
    }
    return os;
}