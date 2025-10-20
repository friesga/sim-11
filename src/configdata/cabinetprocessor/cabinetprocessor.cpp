#include "cabinetprocessor.h"

#include <optional>

using std::optional;
using std::nullopt;

namespace CabinetProcessor
{
    optional<Cabinet::Position> processCabinetKey (iniparser::Value value)
    {
        vector<size_t> items;
        Cabinet::Position result {0, 0_ru};

        std::stringstream ss (value.asString ());
        size_t cabinetNr, height;
        char slash;

        if (!(ss >> cabinetNr >> slash >> height) || slash != '/' || !ss.eof ())
            return {nullopt};

        result.cabinetNr = cabinetNr;
        result.height = height;
    
        return result;
    }
}