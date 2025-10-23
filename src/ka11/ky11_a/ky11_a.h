#ifndef _KY11_A_H_
#define _KY11_A_H_

#include "configdata/ka11/ky11_aconfig/ky11_aconfig.h"
#include "panel.h"
#include "bitfield.h"
#include "types.h"

#include <utility>
#include <string>
#include <array>
#include <functional>

using std::pair;
using std::string;
using std::array;
using std::bind;
using std::placeholders::_1;

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

    static const size_t numberOfSwitches = 18;

    using ButtonNames = pair<string, string>;
    array<ButtonNames, numberOfSwitches> buttonNames_ =
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

    Button* powerSwitch_;
    Button* sr0Button_;
    array<Button*, numberOfSwitches> srButtons_;

    Indicator* runLight_;

    void createBezel (Window* window, const KY11_AConfig& ky11_aConfig);
    void powerSwitchClicked (Button::State state);
    void createSwitchRegisterButtons (unique_ptr<PanelBuilder>& panelBuilder);

    template <size_t buttonIndex>
    void srButtonClicked (Button::State state);

    template <size_t buttonIndex>
    void createSwitchRegisterButton (unique_ptr<PanelBuilder>& panelBuilder);
};

template <size_t buttonIndex>
void KY11_A::createSwitchRegisterButton (unique_ptr<PanelBuilder>& panelBuilder)
{
    srButtons_[buttonIndex] = panelBuilder->createMultiPositionSwitch (
        {buttonNames_[buttonIndex].first, buttonNames_[buttonIndex].second},
        Button::TwoPositionsState::Down,
        bind (&KY11_A::srButtonClicked<buttonIndex>, this, _1));
}

template <size_t buttonIndex>
void KY11_A::srButtonClicked (Button::State state)
{
    switchRegister_.value = (switchRegister_.value & ~(1 << buttonIndex)) |
        ((get<Button::TwoPositionsState> (state) == Button::TwoPositionsState::Up) ?
        (1 << buttonIndex) : 0);
}

// Switch register buttons 16 and 17 were not implemented on the KY11-A panel.
template <>
inline void KY11_A::srButtonClicked<16> (Button::State state)
{}

template <>
inline void KY11_A::srButtonClicked<17> (Button::State state)
{}

#endif // _KY11_A_H_