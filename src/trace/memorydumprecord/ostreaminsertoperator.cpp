#include "memorydumprecord.h"

#include <iomanip>

using std::oct;
using std::setfill;
using std::right;
using std::setw;

// Overloaded operator<< function to print MemoryDump records
std::ostream& operator<< (std::ostream& os, TraceRecord<MemoryDumpRecord> record)
{
    os << "[MEM ] dump " << record.length_ << " from " << std::oct <<
        record.address_ << ":\n";

    for (int i = 0; i < record.length_; ++i)
    {
        if ((i % 8) == 0)
            os << '\n';

        // The u8 type has to be converted to an unsigned int to be printed
        // correctly to cout.
        os << oct << setw(3) << setfill ('0') << unsigned(record.ptr_[i]) << ' ';
    }
    os << "\n";

    return os;
}