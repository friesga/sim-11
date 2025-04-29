#include "unit.h"

// Unit constructor
// Set default values for flags
Unit::Unit (AbstractBusDevice* owningDevice)
    :
    owningDevice_ {owningDevice},
    unitStatus_ {}
{}