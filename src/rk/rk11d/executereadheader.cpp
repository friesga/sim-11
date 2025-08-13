#include "rk11d.h"
#include "rk/include/rktypes.h"

#include <algorithm>

using std::ranges::all_of;

using RKTypes::rk05Geometry_;

void RK11D::executeReadHeader (RKTypes::Function function)
{
    RKTypes::CommandCompletion commandCompletion {};

    all_of (readHeaderFunction_, [&] (auto& f)
        { return f (function, commandCompletion); });
}