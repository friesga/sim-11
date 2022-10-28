#include "unit.h"

// Unit constructor
// Set default values for flags
Unit::Unit (BusDevice *owningDevice)
    :
    owningDevice_ {owningDevice},
    unitStatus_ {Status::UNIT_ATTABLE | Status::UNIT_ROABLE}
{}