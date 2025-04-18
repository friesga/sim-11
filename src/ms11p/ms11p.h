#ifndef _MS11P_H_
#define _MS11P_H_

#include "bus/include/bus.h"
#include "memorydevice.h"
#include "configdata/ms11pconfig/ms11pconfig.h"
#include "bitfield.h"
#include "memorysize.h"

#include <memory>
#include <bit>
#include <array>

using std::unique_ptr;
using std::shared_ptr;
using std::make_unique;
using std::array;

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
    //
    // Bits 11 through 5 of the CSR are used for different purposes. It
    // contains either address bits A17-A11, address bits A21-A18, the
    // check bits (C8-CX) or the syndrome bits (S8-SX). As for these purposes
    // a different number of bits are used, two overlaying fields are
    // defined.
    //
    // Check Bit Storage (Diagnostic Check Bit 2 = 1, Disable Correction Bit 1
    // = Do Not Care) -- These bits are read/write bits in diagnostic check
    // mode. They store the check bits to be written into, or read from,
    // memory. (EK-MS11P-TM-001, par. 2.3.6.1)
    //
    // Diagnostic Register -- This internal six-bit register holds the
    // contents of CSR 5-10. It is loaded on a write CSR or write CSR
    // diagnostic cycle. (EK-MS11P-TM-001, par 3.2.3)
    union csr
    {
        u16 value;
        BitField<u16, 0> uncorrectableErrIndicationEnable;
        BitField<u16, 1> errorCorrectionDisable;
        BitField<u16, 2> diagnosticCheck;
        BitField<u16, 3> inhibitModePointer;
        BitField<u16, 4> singleErrorIndication;
        BitField<u16, 5, 6> checkBitsStorage;
        BitField<u16, 5, 7> errorAddressStorage;
        BitField<u16, 11> a17;
        BitField<u16, 12> notUsed;
        BitField<u16, 13> inhibitModeEnable;
        BitField<u16, 14> eubErrorAddressRetrieval;
        BitField<u16, 15> uncorrectableErrorIndication;
    }
    csr_ {0};

    // Bit 11 (A17) is read-only
    static const u16 writeMask {0173777};
    static const u16 checkBitStorageMask {03740};

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

    // Definition of the MS11-P internal registers. The checkSyndromeBits
    // is used for both check bits and syndrome bits and contains the one
    // or the other, depending on the executed commands.
    BusAddress errorAddress_ {0};
    u8 checkSyndromeBits_ {0};

    enum class BitError
    {
        None,
        Single,
        Double
    };

    // The SyndromeDecodeKey and the syndroneDecodeTable_ contain the
    // information as defined in EK-MS11P-TM-001 Table 3-2.
    enum class SyndromeDecodeKey
    {
        NoError,
        DataBit,
        CheckBit,
        TwoErrors,
        MultipleErrors
    };

    struct SyndromeDecodeCell
    {
        SyndromeDecodeKey key;
        u8 bit;
    };

    // This table decodes the value of the syndrome bits to the bit in error.
    // Each cell contains a pair <SyndromeDecodeKey, error bit number>
    array<SyndromeDecodeCell, 64> syndromeDecodeTable_
    {
        // Column 0
        SyndromeDecodeCell {SyndromeDecodeKey::NoError, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::CheckBit, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::CheckBit, 1},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::CheckBit, 2},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::MultipleErrors, 0},

        // Column 1
        SyndromeDecodeCell {SyndromeDecodeKey::CheckBit, 3},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 1},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::MultipleErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},

        // Column 2
        SyndromeDecodeCell {SyndromeDecodeKey::CheckBit, 4},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 2},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 3},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 4},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},

        // Column 3
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 5},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 6},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 7},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::MultipleErrors, 0},

        // Column 4
        SyndromeDecodeCell {SyndromeDecodeKey::CheckBit, 5},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 8},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 9},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 10},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},

        // Column 5
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 11},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 12},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 13},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::MultipleErrors, 0},

        // Column 6
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 14},
        SyndromeDecodeCell {SyndromeDecodeKey::MultipleErrors},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::DataBit, 15},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::MultipleErrors, 0},

        // Column 7
        SyndromeDecodeCell {SyndromeDecodeKey::MultipleErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::MultipleErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::MultipleErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::MultipleErrors, 0},
        SyndromeDecodeCell {SyndromeDecodeKey::TwoErrors, 0}
    };

    Bus* bus_;
    MS11PConfig::PowerSource powerSource_ {MS11PConfig::PowerSource::System};
    u32 startingAddress_ {0};
    u16 csrAddress_ {0172100};
    u8 syndromeBits_ {0};

    // Definition of the MS11-P memory
    static constexpr MemorySize memorySize_ {512_KiW};
    unique_ptr<u16[]> memory_;
    unique_ptr<u8[]> checkBits_;

    CondData<u16> readCSR ();
    CondData<u16> readMemory (BusAddress address);
    void writeCSR (u16 value);
    void writeMemory (BusAddress address, u16 value);

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
    BitError detectedErrors (u8 syndromeBits);
    u16 correctSingleError (u16 data, u8 syndromeBits);
};

#endif // _MS11P_H_