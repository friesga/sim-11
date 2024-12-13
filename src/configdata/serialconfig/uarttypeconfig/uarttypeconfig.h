#ifndef _UARTTYPECONFIG_H_
#define _UARTTYPECONFIG_H_

// The UARTs are used in different devices, namely the DLV11-J, the KDF11-B
// and the KDF11-U. The UARTs in these devices provide equivalent
// functionality, except for some minor details:
// - The KDF11-U UART has a Maintenance Mode which can be enabled by setting
//   a bit in the XCSR register. In this mode the transmitter output is routed
//   to the receiver input. This feature is used by the M9312 11/24
//   Diagnostic/Console ROM.
//
// As these differences are minor they are implemented as options in a common
// UART source code. The options are configured in this UARTTypeConfig. 
//
struct UARTTypeConfig
{
    bool maintenanceModeSupported {false};
};

#endif // _UARTTYPECONFIG_H_