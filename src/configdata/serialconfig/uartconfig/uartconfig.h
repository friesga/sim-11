#ifndef _UARTCONFIG_H_
#define _UARTCONFIG_H_

#include "types.h"
#include "configdata/deviceconfig/deviceconfig.h"

// This class defines the configuratipn parameters for a DL-compatible
// Universal Asynchronous Receiver-Transmitter (UART).
// 
class UARTConfig : public DeviceConfig
{
    bool enabled;
    u16 baseAddress {};
	u16 vector {};
};

#endif // _UARTCONFIG_H_