#include "unit.h"

// Unit constructor
// Set default values for flags
Unit::Unit (Device *owningDevice)
    :
    owningDevice_ {owningDevice},
    flags_ {UNIT_ATTABLE}
{}