#include "rlv12command.h"
#include "rlv12/rlv12.h"

RLV12Command::RLV12Command (int32_t trackNr, int32_t sectorNr,
    int32_t memoryAddress, int32_t wordCount)
    : 
    trackNumber_ {trackNr},
    sectorNumber_ {sectorNr},
    memoryAddress_ {memoryAddress},
    wordCount_ {wordCount}
{
    int32_t maxWordCount = (RL_NUMSC - sectorNumber_) * RL_NUMWD;

    // Track overrun?
    if (wordCount_ > maxWordCount)                                         
        wordCount_ = maxWordCount;
}

// Accessors
int32_t RLV12Command::trackNumber() const
    { return trackNumber_; }
 
int32_t RLV12Command::sectorNumber() const
    { return sectorNumber_; }

int32_t RLV12Command::memoryAddress() const
    { return memoryAddress_; }

int32_t RLV12Command::wordCount () const
    { return wordCount_; }

// Return the position of the in this command specified track
// as an offset in the file
int32_t RLV12Command::filePosition () const
{
    return (trackNumber_ * RL_NUMSC + sectorNumber_) * RL_NUMWD * 
        sizeof(int16_t);
}

// Default implementation of the finish command for Read, Read no header,
// Write and Write Check commands. This function finishes the execution
// of these commands.
void RLV12Command::finish (RLV12 *controller, RL01_2 *unit)
{
    // Calculate the final word count (i.e. the remaining number of
    // words to be transferred).
    controller->rlmpr = (controller->rlmpr + wordCount_) & 0177777;

    // If the specified transfer could not be completed indicate an error
    // condition
    if (controller->rlmpr != 0)
        controller->rlcs |= RLCS_ERR | RLCS_INCMP | RLCS_HDE;

    memoryAddress_ += (wordCount_ << 1);                                        /* final byte addr */
    
    // Load BAR, CSR and possibly BAE registers with the current address
    controller->memAddrToRegs (memoryAddress_);

    // If we ran off the end of the track, return 40 in rlda, but keep
    // track over a legitimate sector (0)?
    controller->rlda += ((wordCount_ + (RL_NUMWD - 1)) / RL_NUMWD);

    // Update head position
    // ToDo: Remove if statement?!
    if (unit->function_ == RLCS_RNOHDR)
        unit->currentDiskAddress_ = (unit->currentDiskAddress_ & ~RLDA_M_SECT) |
        ((unit->currentDiskAddress_ + ((wordCount_ + (RL_NUMWD - 1)) / RL_NUMWD)) & RLDA_M_SECT);
    else
        unit->currentDiskAddress_ = controller->rlda;

    if (getSector (unit->currentDiskAddress_) >= RL_NUMSC)
        // Wrap to 0 
        unit->currentDiskAddress_ &= ~RLDA_M_SECT;                          

    // RLCSR status error bits are set in execute().
    controller->setDone(0);

    /*
    if (err != 0)
    {
        perror("RL I/O error");
        clearerr(unit.filePtr_);
    }
    */

}

