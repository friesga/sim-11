#ifndef _DLCONFIG_H_
#define _DLCONFIG_H_

#include "types.h"
#include "devicetype.h"
#include "configdata/serialconfig/uartconfig/uartconfig.h"
#include "configdata/serialconfig/consoleconfig/consoleconfig.h"

#include <vector>

using std::vector;

// Set factory configuration for base address, vector and BREAK key response.
// The default break key is set to the esc key.
struct DLV11JConfig : public DeviceType<BusType::QBus>
{
    enum {numChannels = 4};
    enum {defaultBaseAddress = 0176500};
	enum {defaultBaseVector = 0300};
	enum {defaultCh3Address_ = 0177560};
	enum {defaultCh3Vector_ = 060};

    u16 baseAddress {defaultBaseAddress};
	u16 baseVector {defaultBaseVector};
    bool ch3ConsoleEnabled {true};
    bool loopback {true};

    vector<UARTConfig> uarts {};
    ConsoleConfig consoleConfig {};

    DLV11JConfig ();
    void createDLV11J_UARTsConfig ();
};

#endif // _DLCONFIG_H_