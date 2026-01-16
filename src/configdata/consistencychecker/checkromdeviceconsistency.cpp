#include "consistencychecker.h"

#include <algorithm>
#include <stdexcept>
#include <variant>

using std::ranges::find_if;
using std::ranges::count_if;
using std::invalid_argument;
using std::holds_alternative;
using std::visit;

// Check that either a M9312 or a KT24 and not both are specified in the
// configuration
void ConsistencyChecker::checkROMDeviceConsistency ()
{
    size_t numberOfROMS =
        count_if (systemConfig_, &ConsistencyChecker::findDevice<M9312Config>) +
        count_if (systemConfig_, &ConsistencyChecker::findDevice<KT24Config>);

    // Verify just one ROM boot device is specified
    if (numberOfROMS > 1)
        throw invalid_argument {"Double diagnostic/boot ROM device specified, specify either M9312 or KT24"};

    // Verify at least one ROM boot device is specified
    if (numberOfROMS == 0)
        throw invalid_argument {"No diagnostic/boot ROM specified, specify M9312 or KT24"};
}


