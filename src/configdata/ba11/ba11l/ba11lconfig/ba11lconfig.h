#ifndef _BA11LCONFIG_H_
#define _BA11LCONFIG_H_

#include "devicetype.h"
#include "cabinet/cabinet.h"

#include <optional>

using std::optional;
using std::nullopt;

// The BA11-L is the mounting box for several models, at least the PDP-11/04,
// the PDP-11/34 and the PDP-11/24. The mounting box can be provided with
// different front panels, such as the KY11-LA Operator's Console and the
// KY11-LB Programmer's Console. The PDP-11/24 was also available in a BA11-L,
// with an -11/24-specific front panel.
// (Source: https://gunkies.org/wiki/BA11-L_mounting_box)
//
struct BA11_LConfig : public DeviceType<BusType::Unibus>
{
    BA11_LConfig () = default;
    BA11_LConfig (Cabinet::Position cabinetPosition);

    optional<Cabinet::Position> cabinetPosition {nullopt};

    // The BA11-L has a height of three rack units
    static const RackUnit unitHeight;
};

#endif // !_BA11LCONFIG_H_
