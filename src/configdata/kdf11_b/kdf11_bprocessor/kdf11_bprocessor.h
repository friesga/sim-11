#ifndef _KDF11_BPROCESSOR_H_
#define _KDF11_BPROCESSOR_H_

#include "configdata/sectionprocessor/sectionprocessor.h"
#include "../../kd11processor/kd11processor.h"
#include "../kdf11_bconfig/kdf11_bconfig.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

class KDF11_BProcessor : public SectionProcessor
{
public:
	DeviceConfig getConfig () override;

private:
	unique_ptr<KDF11_BConfig> kdf11_bConfigPtr {make_unique<KDF11_BConfig> ()};

	// Define process as a pointer to a KDF11_AProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (KDF11_BProcessor::*Process) (iniparser::Value);
	typedef void (KDF11_BProcessor::*SectionProcess) (iniparser::Section*);
	
	map<string, Process> valueProcessors =
	{
		{"power-up_mode", &KDF11_BProcessor::processPowerUpMode}
	};

	struct SectionProcessDef
	{
		SectionProcess sectionProcessor;
		bool defined {false};
	};

	// A KDF11-B module comprises besides the processor two serial line units
	// and a BDV11-compatible diagnostic and bootstrap module.
	map<string, SectionProcessDef> sectionProcess = 
	{
		{"SLU",   {&KDF11_BProcessor::processSLUSubsection, false}},
		{"BDV11", {&KDF11_BProcessor::processBDV11Subsection, false}}
	};

	void processValue (iniparser::Section::ValueIterator valueIterator) override;
	void checkConsistency () override;
	void processSubsection (iniparser::Section *subSection) override;
	void processPowerUpMode (iniparser::Value value);
	bool sluDefined {false};
	bool bdv11Defined {false};
	void processSLUSubsection (iniparser::Section *subSection);
	void processBDV11Subsection (iniparser::Section *subSection);
};

#endif // _KDF11_BPROCESSOR_H_