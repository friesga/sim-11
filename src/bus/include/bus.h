#ifndef _BUS_H_
#define _BUS_H_

#include "bussignals.h"
#include "businterrupts.h"
#include "busdatatransfer.h"
#include "busconfiguration.h"

// The bus has to perform four different functions:
// 1. Distribute bus signals between the connected devices,
// 2. Priority arbitration between the devices,
// 2. Handle interrupts from the connected devices,
// 3. Transfer data between the connected devices,
// 4. Provide functionality to connect devices to the bus.
//
class Bus : public BusSignals, public BusInterrupts,
    public BusDataTransfer, public BusConfiguration
{
public:
    virtual ~Bus() {}
};

#endif // _BUS_H_