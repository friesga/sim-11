#ifndef _KA11PROCESSOR_H_
#define _KA11PROCESSOR_H_

#include "configdata/sectionprocessor/deviceconfigprocessor.h"
#include "configdata/ka11/ka11config/ka11config.h"

class KA11Processor : public DeviceConfigProcessor
{
public:
	DeviceConfig getConfig () override;

private:
	KA11Config ka11Config_ {};
};


#endif // _KA11PROCESSOR_H_