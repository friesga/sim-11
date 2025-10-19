#include "cabinetprocessor.h"

#include <stdexcept>

using std::invalid_argument;

namespace CabinetProcessor
{

Cabinet::Position processCabinetKey (iniparser::Value value)
{
    vector<size_t> items;
    Cabinet::Position result {0, 0_ru};

    std::stringstream ss (value.asString ());
    size_t cabinetNr, height;
    char slash;

    if (!(ss >> cabinetNr >> slash >> height) || slash != '/' || !ss.eof ())
        throw std::invalid_argument ("Invalid BA11 cabinet position");

    result.cabinetNr = cabinetNr;
    result.height = height;
    
    return result;
}

} // namespace CabinetProcessor