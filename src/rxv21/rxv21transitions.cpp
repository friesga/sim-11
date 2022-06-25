#include "rxv21.h"

// Definition of transitions from the rxv21Idle state

// On the receipt of a rxv21Go event a command to the RX02 is initiated.
// The Maintenance Read Status and Read Error Code commands can be performed
// immediately, the Set Media Density and Write Deleted Data commands are
// currently unsupported. The other commands require additional loading of
// parameters into the RX2DB before they can be executed.
State RXV21::transition (rxv21Idle&&, rxv21Go)
{
    TRCRXV21CMD((rx2cs & RX_FUNCTION_MASK) >> 1, rx2cs);
    rx2cs &= ~RX_GO;

    // Perform the selected function
    // ToDo: Move actions to the state's entry function to have all
    // actions for a specific command in one module?
    switch (rx2cs & RX_FUNCTION_MASK)
    {
        case RX_FILL_BUFFER:
            clearErrors();
            rx2cs &= ~RX_DONE;
            rx2cs |= RX_TR;
            return rxv21FillBufferRx2wc {};
            
        case RX_EMPTY_BUFFER:
            clearErrors();
            rx2cs &= ~RX_DONE;
            rx2cs |= RX_TR;
            return rxv21EmptyBufferRx2wc {};

        case RX_WRITE_SECTOR:
            clearErrors();
            rx2cs &= ~RX_DONE;
            rx2cs |= RX_TR;
            rx2es = RX2ES_DRV_RDY | RX2ES_DRV_DEN;
            return rxv21WriteSectorRx2sa {};

        case RX_READ_SECTOR:
            clearErrors();
            rx2cs &= ~RX_DONE;
            rx2cs |= RX_TR;
            rx2es = RX2ES_DRV_RDY | RX2ES_DRV_DEN;
            return rxv21ReadSectorRx2sa {};

        case RX_SET_MEDIA_DENSITY:
            clearErrors();
            printf("[RXV21] NOT IMPLEMENTED: Set Media Density\n");
            fflush(stdout);
            ::exit(1);

            // Dummy return state to satisfy the compiler
            // ToDo: Provide better alternative for exit call
            return rxv21Idle {};
            
        case RX_READ_STATUS:
            readStatus();
            return rxv21Idle {};

        case RX_WRITE_DELETED_DATA:
            clearErrors();
            printf("[RXV21] NOT IMPLEMENTED: Write Deleted Data\n");
            fflush(stdout);
            ::exit(1);

            // Dummy return state to satisfy the compiler
            // ToDo: Provide better alternative for exit call
            return rxv21Idle {};
            
        case RX_READ_ERROR_CODE:
            rx2cs &= ~RX_DONE;
            rx2cs |= RX_TR;
            rx2es = RX2ES_DRV_RDY | RX2ES_DRV_DEN;
            return rxv21ReadErrorCodeRx2ba {};
    }

    // All possibilities are defined above, so this point cannot
    // be reached.
    printf ("[RXV21] Function code panic");
    reset ();
    return rxv21Idle {};
}