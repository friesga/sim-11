#ifndef _KW11L_H_
#define _KW11L_H_

#include "bus/include/bus.h"
#include "abstractbusdevice/abstractbusdevice.h"
#include "configdata/kw11l/kw11lconfig/kw11lconfig.h"
#include "bitfield.h"

#include <thread>
#include <mutex>

using std::thread;
using std::mutex;

// The KW11-L Line Time Clock is a Unibus device which provides a line time
// clock, producing interrupts at a rate of 50 or 60 Hz. The programming
// interface is extremely simple; there is one status register with two bits:
// one to enable interrupts, and one which is set for every clock cycle
// (and causes an interrupt when that happens, if the other bit is set).
// Source: https://gunkies.org/wiki/KW11-L_Line_Time_Clock
//
class KW11L : public AbstractBusDevice
{
public:
	KW11L (Bus* bus, const KW11LConfig& kw11lConfig);
    ~KW11L ();

	// Functions required by the AbstractBusDevice
	CondData<u16> read (BusAddress address) override;
	StatusCode writeWord (BusAddress address, u16 value) override;
	bool responsible (BusAddress address) override;
	void reset () override;

private:
    // The status register contains just two bits:
    // bit 6 is the interrupt enable bit,
    // bit 7 is the interrupt monitor bit which is set for every clock tick.
    // The other bits are unused and read as zero.
    union STATUSREGISTER
    {
        STATUSREGISTER () : value {0} {}
        STATUSREGISTER (u16 value) : value {value} {}
        u16 value {0};
        BitField<u16, 6> interruptEnable;
        BitField<u16, 7> interruptMonitor;
    } statusRegister_;

	static const u16 statusRegisterAddress {0177546};
    static const u16 vectorAddress {0100};
    static const u16 TICK_RATE {60};


    Bus* bus_;
    thread ltcThread_;
    mutex kw11lMutex_;
    bool running_;

    void tick ();
};

#endif // _KW11L_H_