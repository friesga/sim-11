#ifndef _KDF11_APROCESSOR_H_
#define _KDF11_APROCESSOR_H_

#include "../sectionprocessor/sectionprocessor.h"
#include "../kdf11_aconfig/kdf11_aconfig.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::map;
using std::string;

class KDF11_AProcessor : public SectionProcessor
{
	unique_ptr<KDF11_AConfig> kdf11_aConfigPtr {nullptr};

    // Define process as a pointer to a BDV11Processor member function
	// with a iniparser::Value argument and returning void.

	typedef void (KDF11_AProcessor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{};

    void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);

public:
	KDF11_AProcessor ();
	unique_ptr<DeviceConfig> getConfig ();
};


#endif // _KDF11_APROCESSOR_H_