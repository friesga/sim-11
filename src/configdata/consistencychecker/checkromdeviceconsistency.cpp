#include "consistencychecker.h"

#include <algorithm>
#include <stdexcept>
#include <variant>

using std::ranges::find_if;
using std::ranges::count_if;
using std::invalid_argument;
using std::holds_alternative;
using std::visit;

// Check that one of M9312, KT24 or M792 is specfied in the configuration
void ConsistencyChecker::checkROMDeviceConsistency ()
{
    size_t numberOfROMS =
        count_if (systemConfig_, &ConsistencyChecker::findDevice<M9312Config>) +
        count_if (systemConfig_, &ConsistencyChecker::findDevice<KT24Config>) +
        count_if (systemConfig_, &ConsistencyChecker::findDevice<M792Config>);

    // Verify just one ROM boot device is specified
    if (numberOfROMS > 1)
        throw invalid_argument {"Multiple diagnostic/boot ROM devices specified, specify one of M9312, KT24 or M792"};

    // Verify at least one ROM boot device is specified
    if (numberOfROMS == 0)
        throw invalid_argument {"No diagnostic/boot ROM specified, specify M9312, KT24 or M792"};
}


