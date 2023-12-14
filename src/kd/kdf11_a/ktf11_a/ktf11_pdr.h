#ifndef _KTF11PDR_H_
#define _KTF11PDR_H_

#include "types.h"
#include "basicregister/basicregister.h"

// Definition of KTF11-A Page Descriptor Register
//
// The page descriptor register is a 16-bit register that contains information
// relative to page expansion, page length, and access control.
//
class PDR : public BasicRegister
{
public:
    PDR (u16 value);
    void operator= (u16 const value) override;

private:
    static const u16 AccessControlFieldIndex = 1;
    static const u16 AccessControlFieldMask = (u16) (bitField (2) << AccessControlFieldIndex);
    static const u16 ExpansionDirectionIndex = 3;
    static const u16 ExpansionDirectionMask = bitValue (ExpansionDirectionIndex);
    static const u16 WriteAccessIndex = 6;
    static const u16 WriteAccessMask = bitValue (WriteAccessIndex);
    static const u16 PageLengthFieldIndex = 8;
    static const u16 PageLengthFieldMask = (u16) (bitField (7) << PageLengthFieldIndex);
};

inline PDR::PDR (u16 value)
    : 
    BasicRegister {value}
{}

// All unimplemented bits read as zeros. Note that the W bit is "reset" to "0"
// whenever either PAR or PDR is modified (written into). (EK-KDF11-UG-PR2)
inline void PDR::operator= (u16 const value)
{
    value_ = (value & (AccessControlFieldMask | ExpansionDirectionMask |
        WriteAccessMask | PageLengthFieldMask)) & ~WriteAccessMask;
}

#endif // _KTF11PDR_H_