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
    checkOneBA11 ();
    checkBusConsistency ();
}

// Check that just one BA11, i.e. a BA11-C, a BA11-L or a BA11-N is specified.
void ConsistencyChecker::checkOneBA11 ()
{
    size_t numBA11s = 
        count_if (systemConfig_, &ConsistencyChecker::findDevice<BA11_NConfig>) +
        count_if (systemConfig_, &ConsistencyChecker::findDevice<BA11_LConfig>) +
        count_if (systemConfig_, &ConsistencyChecker::findDevice<BA11_CConfig>);

    if (numBA11s == 0)
        throw invalid_argument {"No BA11 specified, specify a BA11-C, BA11-N or BA11-L"};

    if (numBA11s > 1)
        throw invalid_argument {"Multiple BA11 specification, specify just one BA11"};
}

// Check that a Qbus system does not contain a Unibus device and vice versa
void ConsistencyChecker::checkBusConsistency ()
{
    auto isUnibusDevice = [] (const auto& device)
        {
            return device.isUnibusDevice ();
        };

    auto unibusDevice = [isUnibusDevice] (DeviceConfig device)
        {
            return visit (isUnibusDevice, device);
        };

    auto isQbusDevice = [] (const auto& device)
        {
            return device.isQbusDevice ();
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