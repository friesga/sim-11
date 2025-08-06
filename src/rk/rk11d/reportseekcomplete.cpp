#include "rk11d.h"
#include "trace/trace.h"

// The RK05 uses this function to indicate a seek function completed
//
void RK11D::reportSeekComplete (RKTypes::SeekCompleteReport report)
{
    // seekCompleteQueue_.push (report);
    // pollEventQueue_.push (SeekComplete {});

    trace.debug ("Seek completed for drive " + std::to_string (report.driveId));

    functionQueue_.push (report);
    functionAvailable_.notify_one ();
}