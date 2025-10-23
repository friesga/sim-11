#ifndef _KY11ACONFIG_H_
#define _KY11ACONFIG_H_

#include "cabinet/cabinet.h"

#include <optional>

using std::optional;
using std::nullopt;

struct KY11_AConfig
{
    optional<Cabinet::Position> cabinetPosition {nullopt};

    // The KY11-A panel  has a height of 10.5 inch, i.e. six rack units.
    // The unit height is defined as a static constant to allow the KY11_A
    // struct to be copied with the default copy constructor.
    static const RackUnit unitHeight;
};

#endif // _KY11ACONFIG_H_