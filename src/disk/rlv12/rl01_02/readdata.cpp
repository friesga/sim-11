#include "rl01_02.h"
#include "logger/logger.h"

#include <utility>

using std::pair;
using std::make_pair;

pair<bool, size_t> RL01_02::readData (RLV12Command& rlv12Command, u16* buffer,
    HeadPositionProcedure procedure, u16 diskAddressRegister)
{
    // To avoid nullpointer references, we check if the cartridge is loaded
    // and if not, we return an error. This should not happen however as the
    // RLV12 command processor checks for this condition by a call to available()
    // before calling this function.
    if (cartridge_ == nullptr)
    {
        Logger::instance() << "Error in readData: cartridge not loaded";
        return make_pair (false, 0);
    }
    
    // Revolutional latency is 12.5ms average (EK-RLV-TD-001). 
    alarmClock_.sleepFor (std::chrono::microseconds (12500));

    waitForDriveReady ();

    size_t numBytes = 
        cartridge_->readDataFromSector (darToDiskAddress (rlv12Command.diskAddress_),
        buffer, rlv12Command.wordCount_);

    if (numBytes != 0)
    {
        updateHeadPosition (procedure, rlv12Command.wordCount_, diskAddressRegister);
        return make_pair (true, numBytes);
    }
    else
        return make_pair (false, 0);
}