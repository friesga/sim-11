#ifndef _KDF11_BPROCESSOR_H_
#define _KDF11_BPROCESSOR_H_

#include "../../kd11processor/kd11processor.h"
#include "../kdf11_bconfig/kdf11_bconfig.h"

#include <memory>

using std::unique_ptr;
using std::shared_ptr;
using std::make_unique;

class KDF11_BProcessor : public KD11Processor
{
public:
	KDF11_BProcessor ();
	void processValue (iniparser::Section::ValueIterator valueIterator) override;
	shared_ptr<DeviceConfig> getConfig () override;

private:
	unique_ptr<KDF11_BConfig> kd11ConfigPtr {make_unique<KDF11_BConfig> ()};

	// Define process as a pointer to a KDF11_AProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (KDF11_BProcessor::*Process) (iniparser::Value);
	typedef void (KDF11_BProcessor::*SectionProcess) (iniparser::Section*);
	
	map<string, Process> valueProcessors =
	{
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

	bool sluDefined {false};
	bool bdv11Defined {false};
	void processSubsection (iniparser::Section *subSection) override;
	void processSLUSubsection (iniparser::Section *subSection);
	void processBDV11Subsection (iniparser::Section *subSection);
};

#endif // _KDF11_BPROCESSOR_H_