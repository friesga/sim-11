#include "kdmachinestate.h"
#include "bus/qbus/qbus.h"
#include "proc/kd/common/odt/operatorconsoleaccess/operatorconsoleaccess.h"

#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <iostream>

using std::make_unique;
using std::cerr;

// The factory power-up mode configuration is mode 0 (get vector at address
// 24 and 26), but we'll set it to Bootstrap as that's more convenient for
// the user.
KDMachineState::KDMachineState (Bus* bus, CpuData* cpuData,
    CpuControl* cpuControl, MMU* mmu,
    KD11Config::PowerUpMode powerUpMode, u16 startAddress,
    KD11ODT::Creator odtCreator)
    :
    BaseMachineState (bus, cpuData, cpuControl, mmu),
    powerUpMode_ {powerUpMode},
    startAddress_ {startAddress},
    odtCreator_ {odtCreator}
{}
