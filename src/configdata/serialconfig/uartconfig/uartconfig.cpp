#include "uartconfig.h"

// There is no need for a separate device type for the UARTs.
UARTConfig::UARTConfig (u16 baseAddress, u16 baseVector, bool loopback)
    : 
    enabled_ {true},
    baseAddress_ {baseAddress},
    baseVector_ {baseVector},
    loopback_ {loopback}
{}