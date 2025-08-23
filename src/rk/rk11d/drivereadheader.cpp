#include "rk11d.h"

// Read the given number of headers from the given starting disk address
// into the controller's buffer.
void RK11D::driveReadHeader (RKTypes::Function const& function,
    RKTypes::FunctionResult& functionResult)
{
    u16 driveId = function.diskAddress.driveSelect;

    functionResult = rk05Drives_[driveId]->readHeader (function.diskAddress,
        function.wordCount, buffer_.get ());
}
