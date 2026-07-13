#ifndef _KW11LCONFIG_H_
#define _KW11LCONFIG_H_

#include "devicetype.h"

// The KW11-L is a line time clock for the Unibus. The CSR and vector address
// and bus request level are fixed and the device itself doesn't have any
// configuration options. For the purpose of the simulator a clock source
// option is added which determines whether the simulator clock or the system
// clock (i.e. real time) has to be used as source.
//
// Using the simulator clock as source has the advange that timing for
// execution of diagnostics is accurate. The simulator clock however has
// no relation with the real time and this will cause the program's clock
// to run fast (as the simulator executes PDP-11 instructions faster than the
// original hardware). To prevent this the clock source can be set to the
// system clock but then timing by diagnostics might be inaccurate.
//
struct KW11LConfig : public DeviceType<BusType::Unibus>
{
    enum class ClockSource
    {
        SimulatorClock,
        SystemClock
    };

    explicit KW11LConfig (ClockSource source = ClockSource::SimulatorClock)
        : clockSource {source}
    {}

    ClockSource clockSource;
};

#endif // _KW11LCONFIG_H_