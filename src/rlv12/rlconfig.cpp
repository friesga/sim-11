#include "rlconfig.h"

RLConfig::RLConfig
(u32 baseAddress_, u32 vector, RLType rlType, bool _22bit, size_t numUnits)
    :
    baseAddress_ {baseAddress_},
    vector_ {vector},
    rlType_ {rlType},
    _22bit_ {_22bit},
    numUnits_ {numUnits}
{}