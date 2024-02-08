#ifndef _UARTCONFIG_H_
#define _UARTCONFIG_H_

#include "types.h"
#include "configdata/deviceconfig/deviceconfig.h"

// This class defines the configuratipn parameters for a DL-compatible
// Universal Asynchronous Receiver-Transmitter (UART).
// 
struct UARTConfig : public DeviceConfig
{
    bool enabled_;
    u16 baseAddress_;
	u16 baseVector_;

    UARTConfig (u16 baseAddress, u16 baseVector);
};

#endif // _UARTCONFIG_H_