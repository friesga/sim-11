#include "rl01_02.h"
#include "logger/logger.h"

#include <utility>

using std::pair;
using std::make_pair;

pair<bool, size_t> RL01_02::readData (RLV12Command& rlv12Command, u16* buffer)
{
    // Guard against drive access while a seek is running
    std::lock_guard<std::mutex> guard {driveMutex_};

    // Set position in file to the block to be read
    if (fseek (filePtr_, filePosition (rlv12Command.diskAddress_),
        SEEK_SET))
    {
        Logger::instance () << "Seek error in RL01_02::readData";
        return make_pair (false, 0);
    }

    // Read wordCount * 2 bytes; returned is the number of bytes read 
    size_t numBytes = fread (buffer, sizeof (int16_t), 
        rlv12Command.wordCount_, filePtr_);

    if (ferror (filePtr_))
    {
        Logger::instance () << "Read error in RL01_02::readData";
        return make_pair (false, 0);
    }

    return make_pair (true, numBytes);
}