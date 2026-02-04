#ifndef _KD11ODT_H_
#define _KD11ODT_H_

#include "types.h"
#include "bus/include/bus.h"
#include "proc/include/cpudata.h"
#include "proc/include/cpucontroller.h"
#include "proc/common/datapaths/datapaths.h"
#include "proc/kd/common/odt/operatorconsoleaccess/operatorconsoleaccess.h"

#include <memory>

using std::unique_ptr;

// This class defines the interface for the ODT processors
class KD11ODT
{
public:
    using Creator = function<unique_ptr<KD11ODT>(Bus*, CpuData*, Interfaces::CpuController*,
        DataPaths*, unique_ptr<ConsoleAccess>, bool)>;

    virtual bool processCharacter (u8 character) = 0;
};


#endif // _KD11ODT_H_