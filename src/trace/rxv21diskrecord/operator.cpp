#include "rxv21diskrecord.h"

#include <iomanip>
#include <string>

using std::oct;
using std::setw;
using std::setfill;
using std::string;

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os, TraceRecord<RXV21DiskRecord> record)
{
    const char* name;
    switch (record.type_)
    {
        case RXV21DiskCmd::RXV21_READ:
            name = "read";
            break;

        case RXV21DiskCmd::RXV21_WRITE:
            name = "write";
            break;

        case RXV21DiskCmd::RXV21_WRITE_DD:
            name = "write (delete data)";
            break;

        default:
            name = "???";
            break;
    }
    // fprintf (DST, "[RXV21] %s sector SEC=%d, TR=%d [drive %d, %s density]\n",
    //     name, rx2sa, rx2ta, drive,
    //    density ? "double" : "single");
    // ToDo: density_ should be an enum but that might effect RXV21 code
    string density = record.density_ ? "double" : "single";
    os << "[RXV21] " << name << " sector SEC=" << record.rx2sa_ << 
        ", TR=" << record.rx2ta_ << 
        " [drive " << (unsigned int) record.drive_ <<
        ", " << density << " density]\n";

    return os;
}