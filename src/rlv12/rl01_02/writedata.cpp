#include "rl01_02.h"
#include "logger/logger.h"
#include "rlv12/rlv12const.h"

#include <utility>

using std::pair;
using std::make_pair;

pair<bool, size_t> RL01_02::writeData (RLV12Command& rlv12Command, u16* buffer,
    size_t numWords, HeadPositionProcedure procedure, u16 diskAddressRegister)
{
    // Check the unit is not write-protected
    if (isWriteProtected ())
    {
        driveStatus_ |= RLV12const::MPR_GS_WriteGateError;
        faultIndicator_->show (Indicator::State::On);
        return make_pair (false, 0);
    }

    // Revolutional latency is 12.5ms average (EK-RLV-TD-001). 
    alarmClock_.sleepFor (std::chrono::microseconds (12500));

    waitForDriveReady ();

    size_t numBytes = writeDataToSector (darToDiskAddress (rlv12Command.diskAddress_),
        buffer, numWords);

    updateHeadPosition (procedure, rlv12Command.wordCount_, diskAddressRegister);

    return make_pair (true, numBytes);
}