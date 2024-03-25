#ifndef _DEVICECONFIGPROCESSOR_H_
#define _DEVICECONFIGPROCESSOR_H_

#include "sectionprocessor.h"

// Definition of the interface for device configuration processors.
class DeviceConfigProcessor : public SectionProcessor
{
public:
    virtual DeviceConfig getConfig () = 0;
};

#endif // _DEVICECONFIGPROCESSOR_H_