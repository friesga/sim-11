#include "rl01_02.h"

s32 RL01_02::readHeader ()
{
    // Guard against drive access while a seek is running
    std::lock_guard<std::mutex> guard {driveMutex_};

    return currentDiskAddress_ & 0177777;
}