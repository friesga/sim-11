#ifndef _BA11LCONFIG_H_
#define _BA11LCONFIG_H_

#include "cabinet/cabinet.h"

#include <memory>

using std::shared_ptr;
using std::make_shared;


// The BA11-L is the mounting box for several models, at least the PDP-11/04,
// the PDP-11/34 and the PDP-11/24. The mounting box can be provided with
// different front panels, such as the KY11-LA Operator's Console and the
// KY11-LB Programmer's Console. The PDP-11/24 was also available in a BA11-L,
// with an -11/24-specific front panel.
// (Source: https://gunkies.org/wiki/BA11-L_mounting_box)
//
struct BA11_LConfig
{
    BA11_LConfig () = default;
    BA11_LConfig (Cabinet::Position cabinetPosition);

    shared_ptr<Cabinet::Position> cabinetPosition {nullptr};

    // The BA11-L has a height of three rack units
    static const RackUnit unitHeight;
};

#endif // !_BA11LCONFIG_H_
