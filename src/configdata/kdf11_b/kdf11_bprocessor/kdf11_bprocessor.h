#ifndef _KDF11_BPROCESSOR_H_
#define _KDF11_BPROCESSOR_H_

#include "../../kd11processor/kd11processor.h"
#include "../kdf11_bconfig/kdf11_bconfig.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

class KDF11_BProcessor : public KD11Processor
{
public:
	KDF11_BProcessor ();
	void processValue (iniparser::Section::ValueIterator valueIterator) override;
	unique_ptr<DeviceConfig> getConfig () override;

private:
	unique_ptr<KDF11_BConfig> kd11ConfigPtr {make_unique<KDF11_BConfig> ()};

	// Define process as a pointer to a KDF11_AProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (KDF11_BProcessor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
	};

	bool sluDefined {false};
	void processSubsection (iniparser::Section *subSection) override;
};

#endif // _KDF11_BPROCESSOR_H_