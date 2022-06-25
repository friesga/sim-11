#include "rxv21.h"

// Initiate the command given in the RX2CS register

// On the receipt of a rxv21Go event a command to the RX02 is initiated.
// The Maintenance Read Status command can be performed immediately,
// the Set Media Density and Write Deleted Data commands are currently
// unsupported. The other commands require additional loading of
// parameters into the RX2DB before they can be executed.
State RXV21::transition (rxv21Idle&&, rxv21Go)
{
    TRCRXV21CMD((rx2cs & RX_FUNCTION_MASK) >> 1, rx2cs);
    rx2cs &= ~RX_GO;

    // Perform the selected function, either by executing a transition to
    // the designated state or, in case of the Read Status function, executing
    // the function immediately
    switch (rx2cs & RX_FUNCTION_MASK)
    {
        case RX_FILL_BUFFER:
            return rxv21FillBufferRx2wc {};
            
        case RX_EMPTY_BUFFER:
            return rxv21EmptyBufferRx2wc {};

        case RX_WRITE_SECTOR:
            return rxv21WriteSectorRx2sa {};

        case RX_READ_SECTOR:
            return rxv21ReadSectorRx2sa {};

        case RX_SET_MEDIA_DENSITY:
            clearErrors();
            throw (std::string("[RXV21] NOT IMPLEMENTED: Set Media Density"));
            
        case RX_READ_STATUS:
            readStatus();
            return rxv21Idle {};

        case RX_WRITE_DELETED_DATA:
            clearErrors();
            throw (std::string("[RXV21] NOT IMPLEMENTED: Write Deleted Data"));
            
        case RX_READ_ERROR_CODE:
            return rxv21ReadErrorCodeRx2ba {};
    }

    // All possibilities are defined above, so this "cannot happen"
    throw ("[RXV21] Function code panic");
}