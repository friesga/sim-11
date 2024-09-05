#include "cabinetprocessor.h"

#include <stdexcept>

using std::invalid_argument;
using std::shared_ptr;
using std::make_shared;

namespace CabinetProcessor
{

shared_ptr<Cabinet::Position> processCabinetKey (iniparser::Value value)
{
    vector<size_t> items;
    Cabinet::Position result {0, 0_ru};

    items = split<size_t> (value.asString (), '/');

    if (items.size () != 2)
        throw invalid_argument {"Invalid BA11-N cabinet position"};

    result.cabinetNr = items[0];
    result.height = items[1];
    return make_shared<Cabinet::Position> (result);
}

} // namespace CabinetProcessor