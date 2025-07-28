#ifndef _DRIVEINTERFACE_H_
#define _DRIVEINTERFACE_H_

#include "rktypes.h"
#include "statuscodes.h"

// This class defines a the interface the RK11-D provides to the attached
// RK05 drives.
class DriveInterface
{
public:
    virtual void reportSeekComplete (RKTypes::SeekCompleteReport report) = 0;
    virtual void dataTransferComplete (StatusCode statusCode,
        u16 wordsTransferred, u16 sectorsProcessed) = 0;
};


#endif // _DRIVEINTERFACE_H_