#include "rk05.h"

using std::shared_ptr;

RK05::RK05 (Bus* bus, PDP11Peripheral* controller, shared_ptr<RK05Config> configPtr)
    : Unit (controller)
{
/*
    // Set the unit number
    unitNumber_ = config.unitNumber;
    fileName_ = config.fileName;
    newFile_ = config.newFile;
    writeProtect_ = config.writeProtect;
    overwrite_ = config.overwrite;
    spinUpTime_ = config.spinUpTime;

    // Set the cabinet position
    if (config.cabinetPosition != nullptr)
        cabinetPosition_ = *config.cabinetPosition;
*/
}