#ifndef _KTF11ASR3_H_
#define _KTF11ASR3_H_

#include "types.h"
#include "basicregister/basicregister.h"

// Definition of KTF11-A Status Register 3
//
class SR3 : public BasicRegister
{
public:
    SR3 (u16 value);
    void operator= (u16 const value) override;
    bool _22BitMappingEnabled ();

private:
    static const u16 Enable22BitMappingIndex = 4;
    static const u16 Enable22BitMappingMask = bitValue (Enable22BitMappingIndex);
    static const u16 EnableIOMappingIndex = 5;
    static const u16 EnableIOMappingMask = bitValue (EnableIOMappingIndex);
};

inline SR3::SR3 (u16 value)
    : 
    BasicRegister {value}
{}

// In this register only bits 4 and 5 are writable. This feauture is not
// documented in the KDF11-A User Guide (EK-KDF11-UG-PR2) but is tested in
// diagnostic JKDAD1 TEST14.
inline void SR3::operator= (u16 const value)
{
    value_ = value & (Enable22BitMappingMask | EnableIOMappingMask);
}

inline bool SR3::_22BitMappingEnabled ()
{
    return value_ & Enable22BitMappingMask;
}

#endif // _KTF11ASR3_H_