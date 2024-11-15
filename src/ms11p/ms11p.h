#ifndef _MS11P_H_
#define _MS11P_H_

#include "qbus/qbus.h"
#include "pdp11peripheral/pdp11peripheral.h"
#include "configdata/ms11pconfig/ms11pconfig.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

// Implementation of the MS11-P MOS memory.
//
// The MS11-P (formally, the MS11-PB; if there was an MS11-PA, nothing is
// now known of it) is a Extended Unibus (EUB) MOS DRAM main memory card. As
// an EUB card, it can therefore only be plugged into the EUB slots on the
// PDP-11/24 or PDP-11/44 backplane.
// 
// On power-on, the system is frozen while the entire memory is cleared, to
// prevent spurious ECC errors. For diagnostic purposes, the ECC can be
// disabled, and there are also means for the CPU to read/write the ECC
// bits directly. 
// 
// The board has provision to use battery backup power to retain data during
// a power outage. 
// 
// Source: https://gunkies.org/wiki/MS11-P_MOS_memory
//
class MS11P : public PDP11Peripheral
{
public:
    MS11P (Qbus* bus);
    ~MS11P ();

    // Functions required for the ExtendedUnibusDevice interface
    StatusCode read (BusAddress address, u16* destAddress) override;
    StatusCode writeWord (BusAddress address, u16 value) override;
    bool responsible (BusAddress address) override;

private:
    u32 startingAddress_ {0};
    static const size_t memorySize_ = 1024 * 1024;
    unique_ptr<u8[]> memory_;
};

#endif // _MS11P_H_