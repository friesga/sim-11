#include "cmdprocessor.h"

//
// This functions handles the completion of a data transfer command, i.e.
// a Read Data, Read Data Without Header Check, Write Data or Write Check
// command.
//
u16 CmdProcessor::finishDataTransferCmd (RL01_2 *unit,
    RLV12Command &rlv12Command)
{
    u16 rlcsValue {0};

    // Calculate the final word count (i.e. the remaining number of
    // words to be transferred).
    controller_->rlmpr = 
        (controller_->rlmpr + rlv12Command.wordCount_) & 0177777;

    // If the specified transfer could not be completed indicate an error
    // condition
    if (controller_->rlmpr != 0)
        rlcsValue = RLCS_ERR | RLCS_INCMP | RLCS_HNF;

    // Set memory address to be returned. The requires the memory address
    // not to be changed in the execute functions.
    rlv12Command.memoryAddress_ += (rlv12Command.wordCount_ << 1);
    
    // Load BAR, CSR and possibly BAE registers with the current address
    controller_->memAddrToRegs (rlv12Command.memoryAddress_);

    // If we ran off the end of the track, return 40 in rlda, but keep
    // track over a legitimate sector (0)?
    controller_->rlda += 
        ((rlv12Command.wordCount_ + (RL_NUMWD - 1)) / RL_NUMWD);

    /*
    if (err != 0)
    {
        perror("RL I/O error");
        clearerr(unit.filePtr_);
    }
    */

    return rlcsValue;
}