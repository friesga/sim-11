#ifndef _CABINETPROCESSOR_H_
#define _CABINETPROCESSOR_H_

#include "cabinet/cabinet.h"
#include "../iniprocessor/iniparser.h"

#include <string>
#include <sstream>
#include <vector>
#include <optional>

using std::string;
using std::stringstream;
using std::getline;
using std::vector;
using std::optional;

// This namespace contains functions to process the cabinet key in a
// device configuration. This key can be present for different devices
// hence the separation to a separate namespace.
namespace CabinetProcessor
{
    extern optional<Cabinet::Position> processCabinetKey (iniparser::Value value);
}

#endif // _CABINETPROCESSOR_H_