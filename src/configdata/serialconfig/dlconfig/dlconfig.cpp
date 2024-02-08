#include "dl11config.h"

// A DLConfig object can describe for a DLV11-J or for the serial-line units
// of the KDF11-B. In the first case the device type must be DLV11_J, in the
// second case the device type won't matter.
DLConfig::DLConfig ()
    : 
    DeviceConfig (DeviceType::DLV11_J)
{}