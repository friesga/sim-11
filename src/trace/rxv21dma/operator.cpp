#include "rxv21dma.h"
#include "../rxv21command/rxv21command.h"

#include <iomanip>

using std::oct;
using std::setw;
using std::setfill;

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os, TraceRecord<RXV21Dma> record)
{
    // const char* name = rxv21_get_cmd_name (type);
    // fprintf (DST, "[RXV21] DMA transfer [%s]: %06o words to %06o\n",
    //        name, rx2wc, rx2ba);
    os << "[RXV21] DMA transfer [" << 
        TraceRecord<RXV21Command>::getCommandName (record.type_) << "]: " <<
        oct << setw (6) << setfill('0') << record.rx2wc_ << " words to " <<
        oct << setw (6) << setfill('0') << record.rx2ba_ << '\n';
    return os;
}