#ifndef _MS11P_H_
#define _MS11P_H_

#include "bus/include/bus.h"
#include "memorydevice.h"
#include "configdata/ms11pconfig/ms11pconfig.h"
#include "bitfield.h"
#include "memorysize.h"

#include <memory>
#include <bit>

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
    CondData<u16> read (BusAddress address) override;
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
        BitField<u16, 0> uncorrectableErrIndicationEnable;
        BitField<u16, 1> errorCorrectionDisable;
        BitField<u16, 2> diagnosticCheck;
        BitField<u16, 3> inhibitModePointer;
        BitField<u16, 4> singleErrorIndication;
        BitField<u16, 5, 7> errorAddressAndCheckBits;
        BitField<u16, 11> a17;
        BitField<u16, 12> notUsed;
        BitField<u16, 13> inhibitModeEnable;
        BitField<u16, 14> eubErrorAddressRetrieval;
        BitField<u16, 15> uncorrectableErrorIndication;
    }
    csr_ {0};

    // Bit 11 (A17) is read-only
    static const u16 writeMask {0173777};


    // Definition of the checkbits per memory word
    union CheckBits
    {
        u8 value;
        BitField<u8, 0, 1> CX;
        BitField<u8, 1, 1> C0;
        BitField<u8, 2, 1> C1;
        BitField<u8, 3, 1> C2;
        BitField<u8, 4, 1> C4;
        BitField<u8, 5, 1> C8;
    };

    // Definition of the masks to generate the check bits as defined
    // in EK-MS11P-TM-001 Table 3-1.
    static const u16 cxMask {0b0100101100101110};
    static const u16 c0Mask {0b0001010101010111};
    static const u16 c1Mask {0b1010011010011001};
    static const u16 c2Mask {0b0011100011100011};
    static const u16 c4Mask {0b1100000011111100};
    static const u16 c8Mask {0b1111111100000000};

    struct ErrorLog
    {
        BusAddress address {0};
        u8 syndromeBits {0};
    }
    errorLog_;

    enum class BitError
    {
        None,
        Single,
        Double
    };

    Bus* bus_;
    MS11PConfig::PowerSource powerSource_ {MS11PConfig::PowerSource::System};
    u32 startingAddress_ {0};
    u16 csrAddress_ {0172100};
    u16 syndromeBits_ {077};

    // Definition of the MS11-P memory
    static constexpr MemorySize memorySize_ {512_KiW};
    unique_ptr<u16[]> memory_;
    unique_ptr<u8[]> checkBits_;

    // Definition of the MS11-P modes
    bool diagnosticCheckMode_ {false};

    CondData<u16> readCSR ();
    void writeCSR (u16 value);

    BitError checkParity (BusAddress address, u8 storedCheckBits,
        u8 generatedCheckBits);
    u8 newCheckBits (BusAddress address, u16 value);
    u8 generateCheckBits (u16 word);
    u8 evenParity (u16 word);
    u8 oddParity (u16 word);
    u16 XOR (u16 word, u16 mask);
    u16 NXOR (u16 word, u16 mask);
    bool isEven (u16 word);
    bool isOdd (u16 word);
    bool inhibited (BusAddress address);
    bool inInhibitedFirst16KW (BusAddress address);
    bool inInhibitedSecond16KW (BusAddress address);
    CondData<u16> handleSingleError (BusAddress address, u16 data,
        u8 storedCheckBits, u8 generatedCheckBits);
    CondData<u16> handleDoubleError (BusAddress address, u16 data,
        u8 storedCheckBits, u8 generatedCheckBits);
    u8 addressBitsA17_A11 (BusAddress address);
    u8 addressBitsA21_A18 (BusAddress address);
    u8 generateSyndromeBits (u8 checkBits1, u8 checkBits2);
};

#endif // _MS11P_H_