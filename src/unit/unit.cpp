#include "unit.h"

// Unit constructor
// Set default values for flags
Unit::Unit (PDP11Peripheral *owningDevice)
    :
    owningDevice_ {owningDevice},
    unitStatus_ {Status::UNIT_ROABLE}
{}