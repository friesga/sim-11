#ifndef _MEMORYDEVICE_H_
#define _MEMORYDEVICE_H_

#include "abstractbusdevice/abstractbusdevice.h"

// Interface definition for memory devices
class MemoryDevice : public AbstractBusDevice
{
public:
    virtual u16 loadFile (const char* fileName) = 0;
};

#endif // _MEMORYDEVICE_H_