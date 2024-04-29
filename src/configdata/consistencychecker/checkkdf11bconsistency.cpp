#include "consistencychecker.h"

#include <algorithm>
#include <stdexcept>

using std::ranges::find_if;
using std::invalid_argument;

// Check that not both a KDF11-B and a BDV11 are configured. This would lead
// to a system with undefined behaviour.
void ConsistencyChecker::checkKDF11_BConsistency ()
{
    auto isKDF11_B = [] (DeviceConfig device)
        {return holds_alternative<shared_ptr<KDF11_BConfig>> (device);};
    auto isBDV11 = [] (DeviceConfig device)
        {return holds_alternative<shared_ptr<BDV11Config>> (device);};

    if (find_if (systemConfig_, isKDF11_B) != systemConfig_.end () &&
        find_if (systemConfig_, isBDV11)   != systemConfig_.end ())
            throw invalid_argument {"Specify either KDF11-B or BDV11"}; 
}