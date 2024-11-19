#include "consistencychecker.h"

#include <algorithm>
#include <stdexcept>
#include <variant>

using std::ranges::find_if;
using std::ranges::count_if;
using std::invalid_argument;
using std::holds_alternative;
using std::visit;

void ConsistencyChecker::checkBA11Consistency ()
{
    checkEitherBA11_NOrBA11_L ();
    checkOneBA11 ();
    checkBusConsistency ();
}

// Check that either a BA11-L or a BA11-N and not both are specified
void ConsistencyChecker::checkEitherBA11_NOrBA11_L ()
{
    auto isBA11_N = [] (DeviceConfig device)
        {
            return holds_alternative<shared_ptr<BA11_NConfig>> (device);
        };
    auto isBA11_L = [] (DeviceConfig device)
        {
            return holds_alternative<shared_ptr<BA11_LConfig>> (device);
        };

    if (find_if (systemConfig_, isBA11_N) != systemConfig_.end () &&
        find_if (systemConfig_, isBA11_L) != systemConfig_.end ())
        throw invalid_argument {"Double BA11 specification, specify either BA11-N or BA11-L"};
}

// Check that a BA11-L or a BA11-N is specified
void ConsistencyChecker::checkOneBA11 ()
{
    if (count_if (systemConfig_, &ConsistencyChecker::findDevice<BA11_NConfig>) == 0 &&
        count_if (systemConfig_, &ConsistencyChecker::findDevice<BA11_LConfig>) == 0)
        throw invalid_argument {"No BA11 specified, specify either BA11-N or BA11-L"};
}

// Check that a Qbus system does not contain a Unibus device and vice versa
void ConsistencyChecker::checkBusConsistency ()
{
    auto isUnibusDevice = [] (const auto& device)
        {
            return device->isUnibusDevice ();
        };

    auto unibusDevice = [isUnibusDevice] (DeviceConfig device)
        {
            return visit (isUnibusDevice, device);
        };

    auto isQbusDevice = [] (const auto& device)
        {
            return device->isQbusDevice ();
        };

    auto qbusDevice = [isQbusDevice] (DeviceConfig device)
        {
            return visit (isQbusDevice, device);
        };

    // In the previous consistency checks we have assured that either a BA-L
    // or a BA-N mounting box is present in the configuration. A BA-L has
    // a Unibus backplane and a BA-N has a Qbus backplane.
    if (systemConfig_.isQbusSystem ())
    {
        if (find_if (systemConfig_, unibusDevice) != systemConfig_.end ())
            throw invalid_argument {"A Qbus system cannot contain Unibus devices"};
    }
    else
    {
        // Unibus system
        if (find_if (systemConfig_, qbusDevice) != systemConfig_.end ())
            throw invalid_argument {"A Unibus system cannot contain Qbus devices"};
    }
}