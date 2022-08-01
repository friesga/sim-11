#include "unit.h"

// Unit constructor
// Set default values for flags
Unit::Unit (BusDevice *owningDevice)
    :
    owningDevice_ {owningDevice},
    flags_ {UNIT_ATTABLE | UNIT_ROABLE}
{}