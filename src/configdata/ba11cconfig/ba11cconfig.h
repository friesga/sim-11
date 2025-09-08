#ifndef _BA11CCONFIG_H_
#define _BA11CCONFIG_H_

#include "devicetype.h"
#include "cabinet/cabinet.h"

#include <optional>

using std::optional;

// The BA11-C mounting box was one of the two earliest standard system unit
// backplane mounting boxes produced by DEC, along with the BA11-E mounting
// box. It was used to hold the KA11 CPU and front console (programmer's
// console or 'operator console') for the PDP-11/20 (hence the -C variant
// suffix). 
// Original source: https://gunkies.org/wiki/BA11-C_Mounting_Box
// 
// The BA11-C was also used for other systems, but with different 
// front consoles.
//
struct BA11_CConfig : public DeviceType<BusType::Unibus>
{
    BA11_CConfig () = default;
    BA11_CConfig (Cabinet::Position cabinetPosition);

    optional<Cabinet::Position> cabinetPosition;

    // The BA11-L has a height of 10.5 inch, i.e. six rack units
    static const RackUnit unitHeight;
};

#endif // !_BA11CCONFIG_H_
