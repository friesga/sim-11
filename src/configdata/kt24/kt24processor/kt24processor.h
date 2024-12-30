#ifndef _KT24PROCESSOR_H_
#define _KT24PROCESSOR_H_

#include "configdata/sectionprocessor/deviceconfigprocessor.h"
#include "../kt24config/kt24config.h"

#include <memory>

using std::unique_ptr;

class KT24Processor : public DeviceConfigProcessor
{
public:
	KT24Processor ();
	DeviceConfig getConfig () override;

private:
	unique_ptr<KT24Config> kt24ConfigPtr {nullptr};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section* subSection);
};


#endif // _KT24PROCESSOR_H_