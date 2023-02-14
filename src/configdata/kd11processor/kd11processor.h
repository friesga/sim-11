#ifndef _KD11PROCESSOR_H_
#define _KD11PROCESSOR_H_

#include "../sectionprocessor/sectionprocessor.h"
#include "../kd11config/kd11config.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::map;
using std::string;

class KD11Processor : public SectionProcessor
{
	unique_ptr<KD11Config> kd11ConfigPtr {nullptr};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);

public:
	KD11Processor ();
	unique_ptr<DeviceConfig> getConfig () override;
};

#endif // _KD11PROCESSOR_H_