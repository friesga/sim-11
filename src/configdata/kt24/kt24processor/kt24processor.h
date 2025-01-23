#ifndef _KT24PROCESSOR_H_
#define _KT24PROCESSOR_H_

#include "configdata/sectionprocessor/deviceconfigprocessor.h"
#include "configdata/m9312/m9312processor/m9312processor.h"
#include "../kt24config/kt24config.h"

#include <memory>

using std::unique_ptr;

// The KT24 includes a M9312 and the KT24 options therefore are forwarded
// to a M9312 processor.
class KT24Processor : public DeviceConfigProcessor
{
public:
	KT24Processor ();
	DeviceConfig getConfig () override;
	void processSection (iniparser::Section* section) override;

private:
	unique_ptr<KT24Config> kt24ConfigPtr {nullptr};
	M9312Processor m9312Processor {};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section* subSection);
};


#endif // _KT24PROCESSOR_H_