#ifndef _KDF11_UPROCESSOR_H_
#define _KDF11_UPROCESSOR_H_

#include "configdata/sectionprocessor/deviceconfigprocessor.h"
#include "configdata/kd11processor/kd11processor.h"
#include "../kdf11_uconfig/kdf11_uconfig.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

class KDF11_UProcessor : public DeviceConfigProcessor
{
public:
	DeviceConfig getConfig () override;

private:
	unique_ptr<KDF11_UConfig> kdf11_uConfigPtr {make_unique<KDF11_UConfig> ()};

	// Define process as a pointer to a KDF11_UProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (KDF11_UProcessor::* Process) (iniparser::Value);
	typedef void (KDF11_UProcessor::* SectionProcess) (iniparser::Section*);

	map<string, Process> valueProcessors =
	{
		{"power-up_mode", &KDF11_UProcessor::processPowerUpMode},
		{"kernel_halt",   &KDF11_UProcessor::processKernelHaltMode},
		{"boot_address",  &KDF11_UProcessor::processBootAddress}
	};

	map<string, KDF11_UConfig::PowerUpMode> validPowerUpModes =
	{
		{"bootstrap", KDF11_UConfig::PowerUpMode::Boot},
		{"vector",    KDF11_UConfig::PowerUpMode::Vector}
	};

	map<string, KDF11_UConfig::KernelHaltMode> validKernelHaltModes =
	{
		{"allow_halt", KDF11_UConfig::KernelHaltMode::AllowHalt},
		{"illegal_instruction", KDF11_UConfig::KernelHaltMode::IllegalInstructionTrap}
	};

	void processValue (iniparser::Section::ValueIterator valueIterator) override;
	void checkConsistency () override;
	void processSubsection (iniparser::Section* subSection) override;
	void processPowerUpMode (iniparser::Value value);
	void processKernelHaltMode (iniparser::Value value);
	void processBootAddress (iniparser::Value value);
};

#endif // _KDF11_UPROCESSOR_H_