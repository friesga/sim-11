#include "rk11d.h"

// The RK05 uses this function to indicate a seek function completed
//
void RK11D::reportSeekComplete (RKTypes::SeekCompleteReport report)
{
    seekCompleteQueue_.push (report);
    pollEventQueue_.push (SeekComplete {});
}