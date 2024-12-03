#include "rlv11processor.h"
#include "configdata/rl/rlunitconfig/rlunitconfig.h"
#include "configdata/rl/rlunitprocessor/rlunitprocessor.h"
#include "touint.h"

#include <utility>

using std::make_unique;
using std::move;

RLV11Processor::RLV11Processor ()
{
	rlv11ConfigPtr = make_unique<RLV11Config> ();
}

// This is a RLV11-specific processSection(). This version first calls the
// RL configuration processor to process the common keys and then calls the
// regular ProcessSection() to process the remaining keys. Common keys are
// deleted from the section so they aren't processed by the regular
// ProcessSection().
void RLV11Processor::processSection (iniparser::Section* section)
{
	rlProcessor_.processSection (section);

	SectionProcessor::processSection (section);
}

void RLV11Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	// Throw exception for non-existing key?
	Process processFunction = valueProcessors[valueIterator->first];
	(this->*processFunction)(valueIterator->second);
}

// Check the consistency of the configuration of the RLV1[12] controller
// 
// Currently the simulator only supports the Qbus and therefore the RLV11 
// and RLV12 controllers. When the Unibus is supported the configurator
// should check the controller/bus type consistency.
// 
// The RLV12 controller contains a M1-M2 jumper which, when installed, enables
// the 22-bit option. This option is not present on the RLV11, so allow this
// option only if the controller type is RLV12. 
void RLV11Processor::checkConsistency ()
{
	if (rlv11ConfigPtr->common.rlType == RLConfig::RLType::RL11)
		throw std::invalid_argument
	{"The RL11 can only be configured on Unibus systems"};
}

// Processing of the unit subsections is delegated to the RLProcessor.
void RLV11Processor::processSubsection (iniparser::Section* subSection)
{
	rlProcessor_.processSubsection (subSection);
}

DeviceConfig RLV11Processor::getConfig ()
{
	rlv11ConfigPtr->common = rlProcessor_.getConfig ();
	return move (rlv11ConfigPtr);
}