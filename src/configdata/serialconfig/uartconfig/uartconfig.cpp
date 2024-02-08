#include "uartconfig.h"

// There is no need for a separate device type for the UARTs.
UARTConfig::UARTConfig ()
    : 
    DeviceConfig (DeviceType::DLV11_J)
{}