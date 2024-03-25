#include "pdp11.h"

#include <set>
#include <iostream>
#include <string>
#include <ranges>

using std::vector;
using std::shared_ptr;
using std::set;
using std::cout;
using std::string;
using std::ranges::count_if;

template<typename T>
bool PDP_11::findDevice (DeviceConfig device)
{
    return var_type (device) == typeid (shared_ptr<T> {});   
}

// Check the system configuration for the presence of some indispensible
// controllers, without which the system will not properly run.
// It would be nice if these configuration errors could be indicated
// in a more lifelike way such as setting led indicators.
void PDP_11::checkConsistency (vector<DeviceConfig> systemConfig)
{
    vector<DeviceConfig> presentDevices {};

    // Mark the devices in the systemconfiguration as present
    for (DeviceConfig device : systemConfig)
        presentDevices.push_back (device);

    // Now check for missing devices
    // auto findMSV11 = [] (auto device)
    //    { return var_type (device) == typeid (shared_ptr<MSV11Config> {}); };

    if (count_if (presentDevices, &PDP_11::findDevice<MSV11Config>) == 0)
    // if (count_if (presentDevices, findMSV11) == 0)
        cout << "Warning: No memory configured. This will halt the system.\n";

    size_t numberOFProcessors = 
        count_if (presentDevices, &PDP_11::findDevice<KD11_NAConfig>) +
        count_if (presentDevices, &PDP_11::findDevice<KDF11_AConfig>) +
        count_if (presentDevices, &PDP_11::findDevice<KDF11_BConfig>);

    if (numberOFProcessors == 0)
        throw string("No processor configured, this system cannot run.");

    if (numberOFProcessors > 1)
        throw string("More than one processor configured, this system cannot run.");

    // The system has to contain terminal interfaces and a boot module,
    // either provided by a DLV11-J and a BDV11 or by a KDF11-B.
    if (count_if (presentDevices, &PDP_11::findDevice<KDF11_BConfig>) == 0)
    {
        if (count_if (presentDevices, &PDP_11::findDevice<DLV11JConfig>) == 0)
            throw string("No DLV11 configured, this system cannot run.");

        if (count_if (presentDevices, &PDP_11::findDevice<BDV11Config>) == 0)
            throw string("No BDV11 configured, this system cannot run.");
    }

    if (count_if (presentDevices, &PDP_11::findDevice<RXV21Config>) == 0 && 
        count_if (presentDevices, &PDP_11::findDevice<RLConfig>) == 0)
        cout << "Warning: No removable media configured. This system cannot boot.\n";

    if (count_if (presentDevices, &PDP_11::findDevice<BA11_NConfig>) == 0)
        throw string("No BA11-N Mounting box configured, this system cannot run.");
}