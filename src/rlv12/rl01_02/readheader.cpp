#include "rl01_02.h"

u16 RL01_02::readHeader (RLV12Command& rlv12Command,
    HeadPositionProcedure procedure, u16 diskAddressRegister)
{
    // Guard against drive access while a seek is running
    std::lock_guard<std::mutex> guard {driveMutex_};

    updateHeadPosition (procedure, rlv12Command.wordCount_,
        diskAddressRegister);

    return currentDiskAddress_ & 0177777;
}