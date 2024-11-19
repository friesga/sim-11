#include "consistencychecker.h"

#include <set>
#include <iostream>
#include <string>
#include <ranges>
#include <algorithm>
#include <exception>

using std::vector;
using std::shared_ptr;
using std::set;
using std::cout;
using std::string;
using std::ranges::count_if;
using std::ranges::find_if;
using std::invalid_argument;

ConsistencyChecker::ConsistencyChecker (SystemConfig const& systemConfig)
    :
    systemConfig_ {systemConfig}
{}

// The presence of a BA-L or BA-N mounting box determines if we are dealing
// with a Unibus or a Qbus system. This is the basis for all configuration
// checks and we therefore check first of all if the BA11 configuration is
// in order. We can then determine the type of the system and perform Unibus
// or Qbus checks.
void ConsistencyChecker::checkAll ()
{
    checkBA11Consistency ();

    if (systemConfig_.isQbusSystem ())
        checkQbusConfiguration ();
    else
        checkUnibusConfiguration ();
}

// Check the system configuration for the presence of some indispensible
// controllers, without which the system will not properly run.
// It would be nice if these configuration errors could be indicated
// in a more lifelike way such as setting led indicators.
//
// ToDo: Move checks to separate functions.
void ConsistencyChecker::checkQbusConfiguration ()
{
    vector<DeviceConfig> presentDevices {};

    // Mark the devices in the systemconfiguration as present
    for (DeviceConfig device : systemConfig_)
        presentDevices.push_back (device);

    // Now check for missing devices
    if (count_if (presentDevices, &ConsistencyChecker::findDevice<MSV11Config>) == 0)
        cout << "Warning: No memory configured. This will halt the system.\n";

    size_t numberOFProcessors = 
        count_if (presentDevices, &ConsistencyChecker::findDevice<KD11_NAConfig>) +
        count_if (presentDevices, &ConsistencyChecker::findDevice<KDF11_AConfig>) +
        count_if (presentDevices, &ConsistencyChecker::findDevice<KDF11_BConfig>);

    if (numberOFProcessors == 0)
        throw string("No processor configured, this system cannot run.");

    if (numberOFProcessors > 1)
        throw string("More than one processor configured, this system cannot run.");

    // The system has to contain terminal interfaces and a boot module,
    // either provided by a DLV11-J and a BDV11 or by a KDF11-B.
    if (count_if (presentDevices, &ConsistencyChecker::findDevice<KDF11_BConfig>) == 0)
    {
        if (count_if (presentDevices, &ConsistencyChecker::findDevice<DLV11JConfig>) == 0)
            throw string("No DLV11 configured, this system cannot run.");

        if (count_if (presentDevices, &ConsistencyChecker::findDevice<BDV11Config>) == 0)
            throw string("No BDV11 configured, this system cannot run.");
    }

    if (count_if (presentDevices, &ConsistencyChecker::findDevice<RXV21Config>) == 0 && 
        count_if (presentDevices, &ConsistencyChecker::findDevice<RLConfig>) == 0)
        cout << "Warning: No removable media configured. This system cannot boot.\n";

    if (count_if (presentDevices, &ConsistencyChecker::findDevice<BA11_NConfig>) == 0)
        throw string("No BA11-N Mounting box configured, this system cannot run.");

    checkMS11Consistency<MSV11Config, 64 * 1024> ();

    checkKDF11_BConsistency ();
    checkConsoleConsistency ();
}

void ConsistencyChecker::checkUnibusConfiguration ()
{
    checkMS11Consistency<MS11PConfig, 1024 * 1024> ();
}

// A system with undefined behaviour is created when it is configured
// with two consoles. This occurs when the system comprises a KDF11-B with 
// SLU1 enabled and a DLV11 with ch3_console_enabled.
void ConsistencyChecker::checkConsoleConsistency ()
{
    auto hasKDF11BConsole = [] (DeviceConfig device)
    {
        if (holds_alternative<shared_ptr<KDF11_BConfig>> (device))
        {
            shared_ptr<KDF11_BConfig> kdf11bConfig = 
                get<shared_ptr<KDF11_BConfig>> (device);
            return kdf11bConfig->sluConfig->uartConfig[0].enabled_;
        }
        return false;
    };

    auto hasDLV11JConsole = [] (DeviceConfig device)
    {
        if (holds_alternative<shared_ptr<DLV11JConfig>> (device))
        {
            shared_ptr<DLV11JConfig> dlv11jConfig = 
                get<shared_ptr<DLV11JConfig>> (device);
            return dlv11jConfig->ch3ConsoleEnabled;
        }
        return false;
    };

    if (find_if (systemConfig_, hasKDF11BConsole) != systemConfig_.end () &&
        find_if (systemConfig_, hasDLV11JConsole) != systemConfig_.end ())
            throw invalid_argument {"Specify either KDF11-B or DLV11-J as console"};
}