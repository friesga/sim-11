#ifndef _SWITCHREGISTER_H_
#define _SWITCHREGISTER_H_

#include "panel.h"
#include "bitfield.h"
#include "types.h"

#include <utility>
#include <functional>

using std::pair;
using std::string;
using std::array;
using std::bind;
using std::placeholders::_1;

// The switch register provides a means of manually loading a 16-bit
// address or 16-bit data word into the processor. The processor ignores
// bits 17 and 16; these switchess may be set to either position. For
// addresses using bits 17 and 16, these bits are set within the processor
// if bits 15, 14, and 13 are set. (DEC-11-HR1B-D, Table 3-2).
//
class SwitchRegister
{
public:
    SwitchRegister(unique_ptr<PanelBuilder>& panelBuilder);
    operator u16 () const;

    // Definition of an assignment operator to set the switch register to
    // a specific value. This is useful for unit tests.
    SwitchRegister& operator= (u16 value);

private:
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

    static const size_t numberOfSwitches = 18;

    using ImageNames = pair<string, string>;
    array<ImageNames, numberOfSwitches> buttonNames_ =
    {{
        {"sr00_down", "sr00_up"},
        {"sr01_down", "sr01_up"},
        {"sr02_down", "sr02_up"},
        {"sr03_down", "sr03_up"},
        {"sr04_down", "sr04_up"},
        {"sr05_down", "sr05_up"},
        {"sr06_down", "sr06_up"},
        {"sr07_down", "sr07_up"},
        {"sr08_down", "sr08_up"},
        {"sr09_down", "sr09_up"},
        {"sr10_down", "sr10_up"},
        {"sr11_down", "sr11_up"},
        {"sr12_down", "sr12_up"},
        {"sr13_down", "sr13_up"},
        {"sr14_down", "sr14_up"},
        {"sr15_down", "sr15_up"},
        {"sr16_down", "sr16_up"},
        {"sr17_down", "sr17_up"}
    }};

    array<Button*, numberOfSwitches> srButtons_;

    void createSwitchRegisterButtons (unique_ptr<PanelBuilder>& panelBuilder);
    void setSwitchesToPosition (u16 value);

    template <size_t buttonIndex>
    void srButtonClicked (Button::State state);

    template <size_t buttonIndex>
    void createSwitchRegisterButton (unique_ptr<PanelBuilder>& panelBuilder);
};

template <size_t buttonIndex>
void SwitchRegister::createSwitchRegisterButton (unique_ptr<PanelBuilder>& panelBuilder)
{
    srButtons_[buttonIndex] = panelBuilder->createMultiPositionSwitch (
        {buttonNames_[buttonIndex].first, buttonNames_[buttonIndex].second},
        Button::TwoPositionsState::Down,
        bind (&SwitchRegister::srButtonClicked<buttonIndex>, this, _1));
}

template <size_t buttonIndex>
void SwitchRegister::srButtonClicked (Button::State state)
{
    registerValue_.as_u16 = (registerValue_.as_u16 & ~(1 << buttonIndex)) |
        ((get<Button::TwoPositionsState> (state) == Button::TwoPositionsState::Up) ?
            (1 << buttonIndex) : 0);
}

// Switch register buttons 16 and 17 were not implemented on the KY11-A panel.
template <>
inline void SwitchRegister::srButtonClicked<16> (Button::State state)
{
}

template <>
inline void SwitchRegister::srButtonClicked<17> (Button::State state)
{
}

#endif // _SWITCHREGISTER_H_