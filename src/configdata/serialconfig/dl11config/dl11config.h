#ifndef _DLCONFIG_H_
#define _DLCONFIG_H_

#include "types.h"
#include "devicetype.h"
#include "configdata/serialconfig/uartconfig/uartconfig.h"


// Set factory configuration for base address and vector.
// 
// A DL11 comprises one UART without loopback.
//
struct DL11Config : public DeviceType<BusType::Unibus>
{
    enum { defaultBaseAddress = 0177560 };
    enum { defaultBaseVector = 060 };

    u16 baseAddress {defaultBaseAddress};
    u16 baseVector {defaultBaseVector};

    UARTConfig uart {defaultBaseAddress, defaultBaseVector, false};
};

#endif // _DLCONFIG_H_