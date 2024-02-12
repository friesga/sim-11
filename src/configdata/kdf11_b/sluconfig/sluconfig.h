#ifndef _SLUCONFIG_H_
#define _SLUCONFIG_H_

#include "configdata/serialconfig/uartconfig/uartconfig.h"
#include "configdata/serialconfig/consoleconfig/consoleconfig.h"

#include <vector>

using std::vector;

// This structure defines the configuration parameters for the serial line
// units of a KDF11-B.
struct SLUConfig
{
	enum {defaultSLU1Address = 0177560};
	enum {defaultSLU1Vector = 060};
    enum {defaultSLU2Address = 0176500};
	enum {defaultSLU2Vector = 0300};

    vector<UARTConfig> uartConfig = 
    {
        UARTConfig {defaultSLU1Address, defaultSLU1Vector, true},
        UARTConfig {defaultSLU2Address, defaultSLU2Vector, true},
    };
    ConsoleConfig consoleConfig;

    SLUConfig ();
};

#endif // _SLUCONFIG_H_