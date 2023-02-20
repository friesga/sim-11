#ifndef _DLV11CONFIG_H_
#define _DLV11CONFIG_H_

#include "types.h"
#include "../deviceconfig/deviceconfig.h"

struct DLV11Config : public DeviceConfig
{
    enum class Ch3BreakResponse
    {
        Boot,
        Halt,
        None
    };

    uint16_t baseAddress {0};
	uint16_t vector {0};
    Ch3BreakResponse ch3BreakResponse {Ch3BreakResponse::None};
    unsigned char breakKey {0};

    DLV11Config ();
};

#endif // _DLV11CONFIG_H_