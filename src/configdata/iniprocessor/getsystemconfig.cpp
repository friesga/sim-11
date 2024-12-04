#include "iniprocessor.h"

#include <utility>

SystemConfig IniProcessor::getSystemConfig ()
{
    return std::move (systemConfig);
}