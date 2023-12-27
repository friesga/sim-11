#ifndef _KDF11_APROCESSOR_H_
#define _KDF11_APROCESSOR_H_

#include "../kd11processor/kd11processor.h"
#include "../kdf11_aconfig/kdf11_aconfig.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

class KDF11_AProcessor : public KD11Processor
{
public:
	KDF11_AProcessor ();
	void processValue (iniparser::Section::ValueIterator valueIterator) override;
	unique_ptr<DeviceConfig> getConfig () override;
	void processKTF11_A (iniparser::Value value);

private:
	unique_ptr<KDF11_AConfig> kd11ConfigPtr {make_unique<KDF11_AConfig> ()};

	// Define process as a pointer to a KDF11_AProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (KDF11_AProcessor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"KTF11-A", &KDF11_AProcessor::processKTF11_A}
	};
};

#endif // _KDF11_APROCESSOR_H_