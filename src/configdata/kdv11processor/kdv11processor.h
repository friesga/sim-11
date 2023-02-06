#ifndef _KDV11PROCESSOR_H_
#define _KDV11PROCESSOR_H_

#include "../sectionprocessor/sectionprocessor.h"
#include "../kdv11config/kdv11config.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::map;
using std::string;

class KDV11Processor : public SectionProcessor
{
	unique_ptr<KDV11Config> kdv11ConfigPtr {nullptr};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);

public:
	KDV11Processor ();
	unique_ptr<DeviceConfig> getConfig () override;
};

#endif // _KDV11PROCESSOR_H_