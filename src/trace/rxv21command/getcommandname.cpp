#include "rxv21command.h"

const char* TraceRecord<RXV21Command>::getCommandName (RXV21DiskCmd command)
{
    switch (command)
    {
        case RXV21DiskCmd::RXV21_FILL:
            return "Fill Buffer";

        case RXV21DiskCmd::RXV21_EMPTY:
            return "Empty Buffer";

        case RXV21DiskCmd::RXV21_WRITE:
            return "Write Sector";

        case RXV21DiskCmd::RXV21_READ:
            return "Read Sector";

        case RXV21DiskCmd::RXV21_SET_MD:
            return "Set Media Density";

        case RXV21DiskCmd::RXV21_STATUS:
            return "Read Status";

        case RXV21DiskCmd::RXV21_WRITE_DD:
            return "Write Deleted Data Sector";

        case RXV21DiskCmd::RXV21_READ_ERR:
            return "Read Error Code";

        default:
            return "unknown";
    }
}