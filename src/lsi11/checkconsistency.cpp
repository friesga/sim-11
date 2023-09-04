#include "lsi11.h"

#include <set>
#include <iostream>
#include <string>

using std::vector;
using std::shared_ptr;
using std::set;
using std::cout;
using std::string;


// Check the system configuration for the presence of some indispensible
// controllers, without which the system will not properly run.
// It would be nice if these configuration errors could be indicated
// in a more lifelike way such as setting led indicators.
void LSI11::checkConsistency (vector<shared_ptr<DeviceConfig>> systemConfig)
{
    set<DeviceType> presentDevices {};

    // Mark the devices in the systemconfiguration as present
    for (shared_ptr<DeviceConfig> device : systemConfig)
        presentDevices.insert (device->deviceType_);

    // Now check for missing devices
    if (presentDevices.count(DeviceType::MSV11) == 0)
        cout << "Warning: No memory configured. This will halt the system.\n";

    if (presentDevices.count(DeviceType::KD11_NA) == 0 &&
            presentDevices.count(DeviceType::KDF11_A) == 0)
        throw string("No processor configured, this system cannot run.");

    if (presentDevices.count(DeviceType::KD11_NA) > 0 &&
            presentDevices.count (DeviceType::KDF11_A) > 0)
        throw string("More than one processor configured, this system cannot run.");

    if (presentDevices.count(DeviceType::DLV11_J) == 0)
        throw string("No DLV11 configured, this system cannot run.");

    if (presentDevices.count(DeviceType::BDV11) == 0)
        throw string("No BDV11 configured, this system cannot run.");

    if (presentDevices.count(DeviceType::RXV21) == 0 && 
        presentDevices.count(DeviceType::RLV12) == 0)
        cout << "Warning: No removable media configured. This system cannot boot.\n";

    if (presentDevices.count(DeviceType::BA11_N) == 0)
        throw string("No BA11-N Mounting box configured, this system cannot run.");
}