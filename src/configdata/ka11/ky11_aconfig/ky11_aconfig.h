#ifndef _KY11ACONFIG_H_
#define _KY11ACONFIG_H_

#include "cabinet/cabinet.h"

#include <optional>

using std::optional;
using std::nullopt;

struct KY11_AConfig
{
    optional<Cabinet::Position> cabinetPosition {nullopt};
};

#endif // _KY11ACONFIG_H_