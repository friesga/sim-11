#ifndef _DLCONFIG_H_
#define _DLCONFIG_H_

#include "types.h"
#include "../deviceconfig/deviceconfig.h"
#include "../uartconfig/uartconfig.h"

#include <vector>

using std::vector;

// This class defines the configuration for DL-compatible modules, such
// as the DLV11-J and the serial-line units integrated in the KDF11-B.
class DLConfig : public DeviceConfig
{
    enum class BreakResponse
    {
        Boot,
        Halt,
        None
    };

    BreakResponse BreakResponse {BreakResponse::Halt};
    unsigned char breakKey {27};
    bool loopback {true};
    vector<UARTConfig> uarts;
};

#endif // _DLCONFIG_H_