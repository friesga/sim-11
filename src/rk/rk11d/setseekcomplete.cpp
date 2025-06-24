#include "rk11d.h"

// The RK05 uses this function to indicate a seek function completed
//
void RK11D::setSeekComplete (RKTypes::SeekCompleteReport report)
{
    // Guard against controller register access from the RK11D thread
    std::lock_guard<std::mutex> guard {controllerMutex_};

    driveConditionQueue_.push (report);
    pollEventQueue_.push (SeekComplete {});
}