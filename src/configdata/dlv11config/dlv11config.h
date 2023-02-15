#ifndef _DLV11CONFIG_H_
#define _DLV11CONFIG_H_

#include "../deviceconfig/deviceconfig.h"

struct DLV11Config : public DeviceConfig
{
    enum class Ch3BreakResponse
    {
        Boot,
        Halt,
        None
    };

    Ch3BreakResponse ch3BreakResponse {Ch3BreakResponse::None};
    unsigned char breakKey {0};

    DLV11Config ();
};

#endif // _DLV11CONFIG_H_