#include "unibusmaprecord.h"

#include <iomanip>
#include <string>

using std::oct;
using std::string;

std::ostream& operator<< (std::ostream& os, TraceRecord<UnibusMapRecord> record)
{
    // Get string representation of width
    string width = std::to_string (static_cast<size_t> (record.width_));

    os << std::oct << "[UBM ]" << oct <<
        " Addr=" << record.inputAddress_ << " (" << width << "bit)" <<
        " => " << "idx=" << record.registerIndex_ <<
        " low=" << record.low_ << " high=" << record.high_ << " " <<
        " PA=" << record.physicalAddress_ << '\n';

    return os;
}