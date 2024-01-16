#include "iniprocessor.h"

// This function checks the consistency of the configuration across all
// configured devices.
void IniProcessor::checkConfigConsistency ()
{
    // Check the maximum number of MSV11-D cards in the system
    size_t const maximumNrOfMSV11cards {4};
    size_t nrOfMSV11cards {0};

    for (shared_ptr<DeviceConfig> device : systemConfig)
    {
        if (device->deviceType_ ==  DeviceType::MSV11)
            ++nrOfMSV11cards;
    }

    if (nrOfMSV11cards > maximumNrOfMSV11cards)
        throw std::out_of_range ("Maximum number of MSV11 cards (4) exceeded"); 
}