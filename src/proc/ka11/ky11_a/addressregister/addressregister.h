#ifndef _ADDRESSREGISTER_H_
#define _ADDRESSREGISTER_H_

#include "register.h"
#include "panel.h"
#include "bitfield.h"

#include <utility>
#include <string>

using std::pair;
using std::string;

// [The address register] displays the address in the bus address register
// (BAR) of the processor. This is the address last used by the procerssor
// on the bus.
// 
// The BAR is 18 bits, allowing for future expansion. At present the two most
// significant bits (A17, A16) are ordered according to the lower 16 bits;
// they are only set when bits A15, A14 and A13 are set. Addresses between
// 160000 and 177777, therefore are translated to addresses between 760000
// and 777777, respectively. (DEC-11-HR1B-D, Table 3-1)
//
class AddressRegister : public Register
{
    friend AddressRegister operator+ (AddressRegister const& lhs, u16 const rhs);

public:
    AddressRegister(unique_ptr<PanelBuilder>& panelBuilder);

    // Functions required by the Register interface
    void operator= (u16 const value);
    operator u16 () const;

    AddressRegister& operator+= (u16 const value);

private:
    static const size_t numberOfIndicators = 18;

    array<Indicator*, numberOfIndicators> arIndicators_ {};

    using ImageNames = pair<string, string>;

    array<ImageNames, numberOfIndicators> indicatorNames_ =
    {{
        {"address_00_off", "address_00_on"},
        {"address_01_off", "address_01_on"},
        {"address_02_off", "address_02_on"},
        {"address_03_off", "address_03_on"},
        {"address_04_off", "address_04_on"},
        {"address_05_off", "address_05_on"},
        {"address_06_off", "address_06_on"},
        {"address_07_off", "address_07_on"},
        {"address_08_off", "address_08_on"},
        {"address_09_off", "address_09_on"},
        {"address_10_off", "address_10_on"},
        {"address_11_off", "address_11_on"},
        {"address_12_off", "address_12_on"},
        {"address_13_off", "address_13_on"},
        {"address_14_off", "address_14_on"},
        {"address_15_off", "address_15_on"},
        {"address_16_off", "address_16_on"},
        {"address_17_off", "address_17_on"}
    }};

    union RegisterValue
    {
        u16 as_u16;
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
    registerValue_ {0};

    void createAddressRegisterIndicators (unique_ptr<PanelBuilder>& panelBuilder);
    void setIndicators (u16 value);

    template <size_t indicatorIndex>
    void createAddressRegisterIndicator (unique_ptr<PanelBuilder>& panelBuilder);
};

template <size_t indicatorIndex>
void AddressRegister::createAddressRegisterIndicator (unique_ptr<PanelBuilder>& panelBuilder)
{
    arIndicators_[indicatorIndex] = panelBuilder->createIndicator (
        indicatorNames_[indicatorIndex].first,
        indicatorNames_[indicatorIndex].second,
        Indicator::State::Off);
}

#endif _ADDRESSREGISTER_H_