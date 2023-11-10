#ifndef _DLV11CONFIG_H_
#define _DLV11CONFIG_H_

#include "types.h"
#include "../deviceconfig/deviceconfig.h"

// Set factory configuration for base address, vector and BREAK key response.
// The default break key is set to the esc key.
struct DLV11Config : public DeviceConfig
{
    enum class Ch3BreakResponse
    {
        Boot,
        Halt,
        None
    };

    enum {defaultBaseAddress = 0176500};
	enum {defaultBaseVector = 0300};

    uint16_t baseAddress {defaultBaseAddress};
	uint16_t vector {defaultBaseVector};
    bool ch3ConsoleEnabled {true};
    Ch3BreakResponse ch3BreakResponse {Ch3BreakResponse::Halt};
    unsigned char breakKey {27};

    DLV11Config ();
};

#endif // _DLV11CONFIG_H_