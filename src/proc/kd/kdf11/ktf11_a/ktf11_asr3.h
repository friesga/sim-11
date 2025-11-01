#ifndef _KTF11ASR3_H_
#define _KTF11ASR3_H_

#include "types.h"
#include "basicregister/basicregister.h"
#include "bus/include/bus.h"

// Definition of KTF11-A Status Register 3
//
// SR3 bit <4> enables or disables the memory management 22-bit mapping.
// If memory management is not enabled (SR0 bit 0 is clear), bit 4 is ignored
// and the 16-bit address is not mapped.If memory management is enabled (SR0
// bit 0 is set) and bit 4 is clear, the computer uses 18-bit mapping. If
// memory management is enabled and bit 4 is set, the computer uses 22-bit
// mapping.
//
// SR3 bit <5> is a read/write bit that has no effect on KDF11-BA operation.
// On systems that contain an I/O map (e.g., the PDP-ll/24), bit 5 is set to
// enable I/O map relocation and is cleared to disable relocation. Status
// register 3 is cleared by the RESET instruction, power-up or restart.
// (EK-KDFEB-UG-001)
//
class SR3 : public BasicRegister
{
public:
    SR3 (Bus* bus, u16 value);
    void operator= (u16 const value) override;
    bool _22BitMappingEnabled ();

private:
    static const u16 Enable22BitMappingIndex = 4;
    static const u16 Enable22BitMappingMask = bitValue (Enable22BitMappingIndex);
    static const u16 EnableIOMappingIndex = 5;
    static const u16 EnableIOMappingMask = bitValue (EnableIOMappingIndex);

    Bus* bus_;
};

inline SR3::SR3 (Bus* bus, u16 value)
    : 
    BasicRegister {value},
    bus_ {bus}
{}

// In this register only bits 4 and 5 are writable. This feauture is not
// documented in the KDF11-A User Guide (EK-KDF11-UG-PR2) but is tested in
// diagnostic JKDAD1 TEST14.
inline void SR3::operator= (u16 const value)
{
    value_ = value & (Enable22BitMappingMask | EnableIOMappingMask);

    bus_->IOMapEnable ().set (value & EnableIOMappingMask);
}

inline bool SR3::_22BitMappingEnabled ()
{
    return value_ & Enable22BitMappingMask;
}

#endif // _KTF11ASR3_H_