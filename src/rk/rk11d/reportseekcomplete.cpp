#include "rk11d.h"
#include "trace/trace.h"

// The RK05 uses this function to indicate a seek function completed
//
void RK11D::reportSeekComplete (RKTypes::SeekCompleteReport report)
{
    functionQueue_.push (report);
    functionAvailable_.notify_one ();
}