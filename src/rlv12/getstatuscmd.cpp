#include "rlv12.h"

//
// Perform a Get Status command for the specified unit with the 
// specified parameters.
//
u16 RLV12::getStatusCmd (RL01_02 *unit)
{
    // The Get Status bit (and ToDo: the Marker Bit) in the DAR must
    // be set.
    if (!(dar_ & RLV12const::DAR_GetStatus))
    {
        // Operation incomplete; set error 
        return RLV12const::CSR_CompositeError | RLV12const::CSR_OperationIncomplete;
    }

    // When the get status command is issued with the reset bit set, it will
    // clear the disk drive soft errors (error conditions no longer present)
    // before the status word is transferred back to the controller.
    // (EK-RLV12-TD-001 par. 2.3.1.2)
    if (dar_ & RLV12const::DAR_Reset)
        unit->resetDriveError ();

    // Develop drive state. The result is put in the output buffer, to be
    // retrieved via the MPR.
    dataBuffer_[0] = unit->driveStatus ();

    if (dataBuffer_[0] == RLV12const::MPR_GS_DriveSelectError)
        return RLV12const::CSR_OperationIncomplete;

    /*
    dataBuffer_[0] =
        (u16)(unit->driveStatus_ |
            (unit->currentDiskAddress_ & RLV12const::MPR_GS_HeadSelect));

    // Set Drive Type; a zero indicates an RL01; a one, an RL02.
    if (unit->rlStatus_ & RlStatus::UNIT_RL02)
        dataBuffer_[0] |= RLV12const::MPR_GS_DriveType;

    // Check if unit is write-protected
    if (unit->unitStatus_ & Status::WRITE_PROTECT)
        dataBuffer_[0] |= RLV12const::MPR_GS_WriteLock;

    if (!(unit->unitStatus_ & Status::UNIT_ATT))
    {
        dataBuffer_[0] |= RLV12const::MPR_GS_DriveSelectError;
        unit->driveStatus_ |= RLV12const::MPR_GS_DriveSelectError;
        return RLV12const::CSR_OperationIncomplete;
    }
    return 0;
    */  
    return 0;
}