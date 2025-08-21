#include "rk11d.h"

// Read the given number of headers from the given starting disk address
// into the controller's buffer.
void RK11D::driveReadHeader (RKTypes::Function const& function,
    RKTypes::FunctionResult& functionResult)
{
    u16 driveId = function.diskAddress.driveSelect;
    u32 wordCount = absValueFromTwosComplement (function.wordCount);

    functionResult = rk05Drives_[driveId]->readHeader (function.diskAddress,
        wordCount, buffer_.get ());
}
