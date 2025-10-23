#ifndef _KA11_H_
#define _KA11_H_

#include "bus/include/bus.h"
#include "kd/include/pdp11processor.h"
#include "configdata/ka11/ka11config/ka11config.h"
#include "ka11/ky11_a/ky11_a.h"
#include "panel.h"

#include <memory>

using std::unique_ptr;

// The KA11 is the CPU of the PDP-11/20.
//
class KA11 : public PDP11Processor
{
public:
    KA11 (Bus* bus, Window* window, const KA11Config& ka11Config);

    // Functions required by the BusDevice interface
    CondData<u16> read (BusAddress address);
    StatusCode writeWord (BusAddress address, u16 value);
    StatusCode writeByte (BusAddress address, u8 value);
    bool responsible (BusAddress address);
    void reset ();

    // Functions required by the PDP11Processor interface.
    void start (u16 startAddress);
    void start ();

private:
    Bus* bus_;

    unique_ptr<KY11_A> ky11_a_;
};

#endif _KA11_H_