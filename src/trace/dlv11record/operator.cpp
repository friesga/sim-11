#include "dlv11record.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::oct;
using std::setw;
using std::setfill;
using std::stringstream;
using std::string;

stringstream TraceRecord<DLV11Record>::createMessage (string action)
{
    stringstream os;

    // Convert char to a printable string
    string valueString {};
    valueString += (value_ >= 0x20) ? value_ : '.';
    
    // fprintf (DST, "[DLV11J] CH%d: receive %06o [%c]\n", channel, value, value >= 0x20 ? value : '.')
    os << "[DLV11J] CH" << (unsigned int) channel_ << ": " << action << ' ' <<
            oct << setw(4) << setfill('0') << (unsigned int) value_ <<
            " [" << valueString << "]\n";

    return os;
}

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os, TraceRecord<DLV11Record> record)
{
    // Convert char to a printable string
    string value {};
    value += (record.value_ >= 0x20) ? record.value_ : '.';

    switch (record.type_)
    {
        case DLV11RecordType::DLV11_RESET:
            os << record.createMessage ("reset").str ();
            break;

        case DLV11RecordType::DLV11_RX: 
            os << record.createMessage ("receive").str ();
            break;

        case DLV11RecordType::DLV11_TX:
            os << record.createMessage ("transmit").str ();
            break;

        case DLV11RecordType::DLV11_SET_RXI:
            os << record.createMessage ("set receive interrupt").str ();
            break;

        case DLV11RecordType::DLV11_SET_TXI:
            os << record.createMessage ("set transmit interrupt").str ();
            break;

        case DLV11RecordType::DLV11_CL_RXI:
            os << record.createMessage ("clear receive interrupt").str ();
            break;

        case DLV11RecordType::DLV11_CL_TXI:
            os << record.createMessage ("clear transmit interrupt").str ();
            break;

        case DLV11RecordType::DLV11_OVERRUN:
            os << record.createMessage ("overrun").str ();
            break;

        case DLV11RecordType::DLV11_READER_ENABLE:
            os << record.createMessage ("reader enable").str ();
            break;

        default:
            os << "Unsuported type\n";
            break;
    }

    return os;
}