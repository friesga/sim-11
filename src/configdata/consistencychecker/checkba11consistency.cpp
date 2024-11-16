#include "consistencychecker.h"

#include <algorithm>
#include <stdexcept>

using std::ranges::find_if;
using std::ranges::count_if;
using std::invalid_argument;

void ConsistencyChecker::checkBA11Consistency ()
{
    checkEitherBA11_NOrBA11_L ();
    checkOneBA11 ();
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