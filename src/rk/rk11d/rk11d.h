#ifndef _RK11D_H_
#define _RK11D_H_

#include "pdp11peripheral/pdp11peripheral.h"
#include "configdata/rk/rk11d/rk11dconfig/rk11dconfig.h"
#include "panel.h"

class RK11D : public PDP11Peripheral
{
private:
    // Define RK11-D registers as offsets from the controller's base address
    enum
    {
        RKDS = 00,      // Drive Status register
        RKER = 02,      // Error register
        RKCS = 04,      // Control Status register
        RKWC = 06,      // Word Count register
        RKBA = 010,     // Current Bus Address register
        RKDA = 012,     // Disk Address register
        RKDB = 014,     // Data Buffer register
    };

public:
	RK11D (Bus* bus, RK11DConfig& rk11dConfig);
    RK11D (Bus* bus, Window* window, RK11DConfig& rk11dConfig);
	CondData<u16> read (BusAddress address) override;
	StatusCode writeWord (BusAddress address, u16 value) override;
	bool responsible (BusAddress address) override;
	void reset () override;


};


#endif // _RK11D_H_