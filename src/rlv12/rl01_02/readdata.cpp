#include "rl01_02.h"
#include "logger/logger.h"

#include <utility>

using std::pair;
using std::make_pair;

pair<bool, size_t> RL01_02::readData (RLV12Command& rlv12Command, u16* buffer,
    HeadPositionProcedure procedure, u16 diskAddressRegister)
{
    // Revolutional latency is 12.5ms average (EK-RLV-TD-001). 
    alarmClock_.sleepFor (std::chrono::microseconds (12500));

    drive_.waitForDriveReady ();

    // Set position in file to the block to be read
    if (fseek (filePtr_, filePosition (rlv12Command.diskAddress_),
        SEEK_SET) != 0)
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

    updateHeadPosition (procedure, rlv12Command.wordCount_, diskAddressRegister);

    return make_pair (true, numBytes);
}