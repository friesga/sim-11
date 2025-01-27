#ifndef _MS11P_H_
#define _MS11P_H_

#include "bus/include/bus.h"
#include "memorydevice.h"
#include "configdata/ms11pconfig/ms11pconfig.h"

#include <memory>

using std::unique_ptr;
using std::shared_ptr;
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
class MS11P : public MemoryDevice
{
public:
    MS11P (Bus* bus);
    MS11P (Bus* bus, shared_ptr<MS11PConfig> ms11pConfig);
    ~MS11P ();

    // Functions required for the BusDevice interface
    StatusCode read (BusAddress address, u16* destAddress) override;
    StatusCode writeWord (BusAddress address, u16 value) override;
    bool responsible (BusAddress address) override;
    void reset () override;

    // Function required for the MemoryDevice interface
    u16 loadFile (const char* fileName);

    void BPOKReceiver (bool signalValue);

private:
    // CSR bit definitions
    static const u16 UncorrectableErrIndicationEnable   = 1 << 0;
    static const u16 ErrorCorrectionDisable             = 1 << 1;
    static const u16 DiagnosticCheck                    = 1 << 2;
    static const u16 InhibitModePointer                 = 1 << 3;
    static const u16 SingleErrorIndication              = 1 << 4;
    static const u16 A17                                = 1 << 11;
    static const u16 InhibitModeEnable                  = 1 << 13;
    static const u16 EUBErrorAddressRetrieval           = 1 << 14;
    static const u16 OncorrectableErrorIndication       = 1 << 15;

    static const u16 SyndromeBitsMask                   = 03740;

    Bus* bus_;
    MS11PConfig::PowerSource powerSource_ {MS11PConfig::PowerSource::System};
    u32 startingAddress_ {0};
    u16 csrAddress_ {0172100};
    u16 csr_ {0};
    static const size_t memorySize_ = 1024 * 1024;
    unique_ptr<u8[]> memory_;

    void readCSR (u16* destAddress);
    void writeCSR (u16 value);
};

#endif // _MS11P_H_