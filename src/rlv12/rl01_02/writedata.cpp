#include "rl01_02.h"
#include "logger/logger.h"

#include <utility>

using std::pair;
using std::make_pair;

pair<bool, size_t> RL01_02::writeData (RLV12Command& rlv12Command, u16* buffer,
    size_t numWords)
{
    // Revolutional latency is 12.5ms average (EK-RLV-TD-001). 
    alarmClock_.sleepFor (std::chrono::microseconds (12500));

    // Guard against drive access while a seek is running
    std::lock_guard<std::mutex> guard {driveMutex_};

    // Set position in file to the block to be written
    if (fseek (filePtr_, filePosition (rlv12Command.diskAddress_), SEEK_SET) != 0)
    {
        Logger::instance () << "Seek error in RL01_02::writeData";
        return make_pair (false, 0);
    }

    size_t numBytes = fwrite (buffer, sizeof (int16_t), numWords, filePtr_);

    if (ferror (filePtr_))
    {
        Logger::instance () << "Write error in RL01_02::writeDataCmd";
        return make_pair (false, 0);
    }

    return make_pair (true, numBytes);
}