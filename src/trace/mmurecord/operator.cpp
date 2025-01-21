#include "mmurecord.h"

#include <iomanip>
#include <string>

using std::oct;
using std::dec;
using std::string;

std::ostream& operator<< (std::ostream& os, TraceRecord<MMURecord> record)
{
    // Get string representation of width
    string width = std::to_string (static_cast<size_t> (record.width_));

    os << "[MMU ]" << " Address=" << oct << record.address_ <<
        " (" << dec << width << "bit)" << '\n';

    return os;
}