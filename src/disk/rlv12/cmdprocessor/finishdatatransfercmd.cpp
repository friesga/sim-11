#include "cmdprocessor.h"

//
// This functions handles the completion of a data transfer command, i.e.
// a Read Data, Read Data Without Header Check, Write Data or Write Check
// command.
//
u16 CmdProcessor::finishDataTransferCmd (RL01_02 *unit,
    RLV12Command &rlv12Command)
{
    u16 rlcsValue {0};

    // Calculate the final word count (i.e. the remaining number of
    // words to be transferred).
    controller_->wordCounter_ = 
        (controller_->wordCounter_ + rlv12Command.wordCount_) & 0177777;

    // If the specified transfer could not be completed indicate an error
    // condition. This occurs on partial read/writes when data would be
    // transferred past the end of the last sector of a track.
    // Refer to EK-RLV12-TD-001, pg. 3-26. XXDP diagnostic XRLHB0 test 22
    // will test this situation and expects a Header Not Found error.
    if (controller_->wordCounter_ != 0)
        rlcsValue = RLV12const::CSR_OperationIncomplete | RLV12const::CSR_HeaderNotFound;

    // Set memory address to be returned. The requires the memory address
    // not to be changed in the execute functions.
    rlv12Command.memoryAddress_ += (rlv12Command.wordCount_ << 1);
    
    // Load BAR, CSR and possibly BAE registers with the current address
    controller_->memAddrToRegs (rlv12Command.memoryAddress_);

    // If we ran off the end of the track, return 40 in rlda, but keep
    // track over a legitimate sector (0)?
    controller_->dar_ += 
        ((rlv12Command.wordCount_ + 
            (RLV12const::wordsPerSector - 1)) / RLV12const::wordsPerSector);

    return rlcsValue;
}