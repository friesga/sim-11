#ifndef _MS11P_H_
#define _MS11P_H_

#include "bus/include/bus.h"
#include "memorydevice.h"
#include "configdata/ms11pconfig/ms11pconfig.h"
#include "bitfield.h"
#include "memorysize.h"

#include <memory>

using std::unique_ptr;
using std::shared_ptr;
using std::make_unique;

// Implementation of the MS11-P MOS memory.
//
// The MS11-PB (M8743-BA) is a metal oxide semiconductor (MOS) random access
// memory (RAM), which provides 512K x 16 bits of data storage. The MS11-PB is
// designed for use with the PDP-11 extended Unibus. (EK-MS11P-TM-001)
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
    // An initialization will apply to the first element in the union.
    union csr
    {
        u16 value;
        BitField<u16, 0, 1> uncorrectableErrIndicationEnable;
        BitField<u16, 1, 1> errorCorrectionDisable;
        BitField<u16, 2, 1> diagnosticCheck;
        BitField<u16, 3, 1> inhibitModePointer;
        BitField<u16, 4, 1> singleErrorIndication;
        BitField<u16, 5, 7> errorAddressAndCheckBits;
        BitField<u16, 11, 1> a17;
        BitField<u16, 12, 1> notUsed;
        BitField<u16, 13, 1> inhibitModeEnable;
        BitField<u16, 14, 1> eubErrorAddressRetrieval;
        BitField<u16, 15, 1> uncorrectableErrorIndication;
    }
    csr_ {0};

    Bus* bus_;
    MS11PConfig::PowerSource powerSource_ {MS11PConfig::PowerSource::System};
    u32 startingAddress_ {0};
    u16 csrAddress_ {0172100};
    u16 syndromeBits_ {077};

    static constexpr MemorySize memorySize_ {512_KiW};
    unique_ptr<u16[]> memory_;

    void readCSR (u16* destAddress);
    void writeCSR (u16 value);
};

#endif // _MS11P_H_