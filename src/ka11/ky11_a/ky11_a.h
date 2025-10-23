#ifndef _KY11_A_H_
#define _KY11_A_H_

#include "configdata/ka11/ky11_aconfig/ky11_aconfig.h"
#include "panel.h"
#include "bitfield.h"
#include "types.h"

class KY11_A
{
public:
    KY11_A (Window* window, const KY11_AConfig& ky11_aConfig);

private:
    // The switch register provides a means of manually loading a 16-bit
    // address or 16-bit data word into the processor. The processor ignores
    // bits 17 and 16; these switchess may be set to either position. For
    // addresses using bits 17 and 16, these bits are set within the processor
    // if bits 15, 14, and 13 are set. (DEC-11-HR1B-D, Table 3-2).
    //
    union SwitchRegister
    {
        u16 value;
        BitField<u16, 0> _0;
        BitField<u16, 1> _1;
        BitField<u16, 2> _2;
        BitField<u16, 3> _3;
        BitField<u16, 4> _4;
        BitField<u16, 5> _5;
        BitField<u16, 6> _6;
        BitField<u16, 7> _7;
        BitField<u16, 8> _8;
        BitField<u16, 9> _9;
        BitField<u16, 10> _10;
        BitField<u16, 11> _11;
        BitField<u16, 12> _12;
        BitField<u16, 13> _13;
        BitField<u16, 14> _14;
        BitField<u16, 15> _15;
    }
    switchRegister_ {0};

    Button* powerSwitch_;
    Button* sr0Button_;

    Indicator* runLight_;

    void createBezel (Window* window, const KY11_AConfig& ky11_aConfig);
    void powerSwitchClicked (Button::State state);
    void sr0ButtonClicked (Button::State state);
};

#endif // _KY11_A_H_