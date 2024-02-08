#ifndef _DLCONFIG_H_
#define _DLCONFIG_H_

#include "types.h"
#include "configdata/deviceconfig/deviceconfig.h"

// Set factory configuration for base address, vector and BREAK key response.
// The default break key is set to the esc key.
struct DLConfig : public DeviceConfig
{
    enum class BreakResponse
    {
        Boot,
        Halt,
        None
    };

    enum {defaultBaseAddress = 0176500};
	enum {defaultBaseVector = 0300};

    u16 baseAddress {defaultBaseAddress};
	u16 baseVector {defaultBaseVector};
    bool ch3ConsoleEnabled {true};
    BreakResponse breakResponse {BreakResponse::Halt};
    unsigned char breakKey {27};
    bool loopback {true};

    DLConfig ();
};

#endif // _DLCONFIG_H_