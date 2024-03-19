#include "pdp11.h"

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
void PDP_11::checkConsistency (vector<DeviceConfigVariant> systemConfig)
{
    set<DeviceConfigVariant> presentDevices {};

    // Mark the devices in the systemconfiguration as present
    for (DeviceConfigVariant device : systemConfig)
        presentDevices.insert (device);

    // Now check for missing devices
    if (presentDevices.count(shared_ptr<MSV11Config> {}) == 0)
        cout << "Warning: No memory configured. This will halt the system.\n";

    size_t numberOFProcessors = 
        presentDevices.count (shared_ptr<KD11_NAConfig> {}) +
        presentDevices.count (shared_ptr<KDF11_AConfig> {}) +
        presentDevices.count (shared_ptr<KDF11_BConfig> {});

    if (numberOFProcessors == 0)
        throw string("No processor configured, this system cannot run.");

    if (numberOFProcessors > 1)
        throw string("More than one processor configured, this system cannot run.");

    // The system has to contain terminal interfaces and a boot module,
    // either provided by a DLV11-J and a BDV11 or by a KDF11-B.
    if (presentDevices.count (shared_ptr<KDF11_BConfig> {}) == 0)
    {
        if (presentDevices.count(shared_ptr<DLV11JConfig> {}) == 0)
            throw string("No DLV11 configured, this system cannot run.");

        if (presentDevices.count(shared_ptr<BDV11Config> {}) == 0)
            throw string("No BDV11 configured, this system cannot run.");
    }

    if (presentDevices.count(shared_ptr<RXV21Config> {}) == 0 && 
        presentDevices.count(shared_ptr<RLConfig> {}) == 0)
        cout << "Warning: No removable media configured. This system cannot boot.\n";

    if (presentDevices.count(shared_ptr<BA11_NConfig> {}) == 0)
        throw string("No BA11-N Mounting box configured, this system cannot run.");
}