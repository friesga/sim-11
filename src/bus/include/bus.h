#ifndef _BUS_H_
#define _BUS_H_

#include "bussignals.h"
#include "businterrupts.h"
#include "busdatatransfer.h"
#include "busconfiguration.h"

// The bus has to perform four different functions:
// 1. It has to distribute bus signals between the connected devices,
// 2. It has to handle interrupts from the connected devices,
// 3. It has to transfer data between the connected devices,
// 4. It has to provide finctionality to connect devices to the bus.
//
class Bus : public BusSignals, public BusInterrupts,
    public BusDataTransfer, public BusConfiguration
{
public:
    virtual ~Bus() {}
};

#endif // _BUS_H_