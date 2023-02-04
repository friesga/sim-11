#ifndef _BA11NPROCESSOR_H_
#define _BA11NPROCESSOR_H_

#include "../sectionprocessor/sectionprocessor.h"
#include "../ba11nconfig/ba11nconfig.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::map;
using std::string;

class BA11_NProcessor : public SectionProcessor
{
	unique_ptr<BA11_NConfig> ba11_nConfigPtr {nullptr};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);

public:
	BA11_NProcessor ();
	unique_ptr<DeviceConfig> getConfig () override;
};

#endif // !_BA11NPROCESSOR_H_
