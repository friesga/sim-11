#ifndef _DRIVEINTERFACE_H_
#define _DRIVEINTERFACE_H_

#include "rktypes.h"

// This class defines a the interface the RK11-D provides to the attached
// RK05 drives.
class DriveInterface
{
public:
    virtual void setDriveCondition (RKTypes::DriveCondition condition) = 0;
};


#endif // _DRIVEINTERFACE_H_