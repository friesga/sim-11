#include "consistencychecker.h"

#include <algorithm>
#include <stdexcept>

using std::ranges::find_if;
using std::invalid_argument;

void ConsistencyChecker::checkBA11Consistency ()
{
    auto isBA11_N = [] (DeviceConfig device)
        { return holds_alternative<shared_ptr<BA11_NConfig>> (device); };
    auto isBA11_L = [] (DeviceConfig device)
        { return holds_alternative<shared_ptr<BA11_LConfig>> (device); };

    if (find_if (systemConfig_, isBA11_N) != systemConfig_.end () &&
        find_if (systemConfig_, isBA11_L) != systemConfig_.end ())
        throw invalid_argument {"Double BA11 specification, specify either BA11-N or BA11-L"};
}