#ifndef _DEVICECONFIG_H_
#define _DEVICECONFIG_H_

#include "../devicetype/devicetype.h"

struct DeviceConfig
{
    DeviceType deviceType_;

    // Define a constructor to be able to set the deviceType from the
    // derived class constructors
    DeviceConfig (DeviceType devicetype);
};

#endif // _DEVICECONFIG_H_