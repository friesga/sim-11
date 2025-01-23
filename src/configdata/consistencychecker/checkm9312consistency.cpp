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
void ConsistencyChecker::checkM9312Consistency ()
{
    checkNotBothM9312AndKT24 ();
    checkM9312OrKT24 ();
}

void ConsistencyChecker::checkNotBothM9312AndKT24 ()
{
    auto isM9312 = [] (DeviceConfig device)
        {
            return holds_alternative<shared_ptr<M9312Config>> (device);
        };
    auto isKT24 = [] (DeviceConfig device)
        {
            return holds_alternative<shared_ptr<KT24Config>> (device);
        };

    if (find_if (systemConfig_, isM9312) != systemConfig_.end () &&
        find_if (systemConfig_, isKT24) != systemConfig_.end ())
        throw invalid_argument {"Double diagnostic/boot ROM device specified, specify either M9312 or KT24"};
}

// Check that a BA11-L or a BA11-N is specified
void ConsistencyChecker::checkM9312OrKT24 ()
{
    if (count_if (systemConfig_, &ConsistencyChecker::findDevice<BA11_NConfig>) == 0 &&
        count_if (systemConfig_, &ConsistencyChecker::findDevice<BA11_LConfig>) == 0)
        throw invalid_argument {"No diagnostic/boot ROM specified, specify M9312 or KT24"};
}