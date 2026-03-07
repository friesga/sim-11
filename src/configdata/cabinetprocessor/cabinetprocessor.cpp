#include "cabinetprocessor.h"

#include <optional>

using std::optional;
using std::nullopt;

namespace CabinetProcessor
{
    optional<Cabinet::Position> processCabinetKey (iniparser::Value value)
    {
        std::stringstream ss (value.asString ());
        size_t cabinetNr, height;
        char slash;

        if (!(ss >> cabinetNr >> slash >> height) || slash != '/' || !ss.eof ())
            return {nullopt};
    
        return Cabinet::Position {cabinetNr, height};
    }
}