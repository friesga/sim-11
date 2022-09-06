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
