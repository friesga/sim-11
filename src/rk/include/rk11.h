#ifndef _RK11_H_
#define _RK11_H_

#include "abstractbusdevice/abstractbusdevice.h"
#include "rk/include/rkdefinitions.h"

// Interface definition for RK(V)11 devices.
//
class RK11 : public AbstractBusDevice
{
    virtual void processResult (RKDefinitions::Result result) = 0;
};

#endif // _RK11_H_