#include "cabinetprocessor.h"

#include <stdexcept>

using std::invalid_argument;

namespace CabinetProcessor
{

Cabinet::Position processCabinetKey (iniparser::Value value)
{
    vector<size_t> items;
    Cabinet::Position result {0, 0_ru};

    items = split<size_t> (value.asString (), '/');

    if (items.size () != 2)
        throw invalid_argument {"Invalid BA11-N cabinet position"};

    result.cabinetNr = items[0];
    result.height = items[1];
    return result;
}

} // namespace CabinetProcessor