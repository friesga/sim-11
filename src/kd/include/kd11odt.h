#ifndef _KD11ODT_H_
#define _KD11ODT_H_

#include "types.h"
#include "qbus/qbus.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/include/mmu.h"
#include "kd/common/operatorconsoleaccess/operatorconsoleaccess.h"

#include <memory>

using std::unique_ptr;

// This class defines the interface for the ODT processors
class KD11ODT
{
public:
    using Creator = function<unique_ptr<KD11ODT>(Qbus*, CpuData*, CpuControl*,
        MMU*, unique_ptr<ConsoleAccess>)>;

    virtual bool processCharacter (u8 character) = 0;
};


#endif // _KD11ODT_H_