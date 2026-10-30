#include "cmdprocessor.h"

//
// The RLO1/RL02 disk drive will not do spiral read/writes. If data is to be
// transferred past the end of the last sector of a track, it is necessary
// to break up the operation [...] (EK-RLV12-TD-001, pg. 3-26).
//
// This restriction limits the word count to the number of words in the
// the number of sectors (40) minus the starting sector number of sectors.
//
void CmdProcessor::limitWordCount (RLV12Command &rlv12Command)
{
    size_t maxWordCount = 
        (RLV12const::sectorsPerSurface - 
            RLV12const::getSector (rlv12Command.diskAddress_)) *
            RLV12const::wordsPerSector;

    if (rlv12Command.wordCount_ > maxWordCount)
        rlv12Command.wordCount_ = maxWordCount;
}