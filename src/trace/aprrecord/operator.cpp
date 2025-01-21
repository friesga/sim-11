#include "aprrecord.h"

#include <iomanip>

using std::oct;
using std::setw;
using std::setfill;
using std::endl;

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os, TraceRecord<APRRecord> record)
{
    os << "[APR ] ";
    for (size_t index = 0; index < 8; ++index)
    {
        os << "KPAR" << index << '=' << 
            oct << setw (6) << setfill ('0') <<
            record.aprSet_.activePageRegister_[index].pageAddressRegister_ << ' ';
    }
    os << "\n       ";

    for (size_t index = 0; index < 8; ++index)
    {
        os << "KPDR" << index << '=' << 
            oct << setw (6) << setfill ('0') <<
            record.aprSet_.activePageRegister_[index].pageDescripterRegister_ << ' ';
    }
    os << endl;

    return os;
}