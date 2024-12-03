#include "rl11processor.h"
#include "configdata/rl/rlunitconfig/rlunitconfig.h"
#include "configdata/rl/rlunitprocessor/rlunitprocessor.h"
#include "touint.h"

#include <utility>

using std::make_unique;
using std::move;

RL11Processor::RL11Processor ()
{
	rl11ConfigPtr = make_unique<RL11Config> ();
}

// This is a RL11-specific processSection(). This version first calls the
// RL configuration processor to process the common keys and then calls the
// regular ProcessSection() to process the remaining keys. Common keys are
// deleted from the section so they aren't processed by the regular
// ProcessSection().
void RL11Processor::processSection (iniparser::Section* section)
{
	rlProcessor_.processSection (section);

	SectionProcessor::processSection (section);
}

void RL11Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{

	Process processFunction = valueProcessors[valueIterator->first];
	if (processFunction == nullptr)
		// This exception will be catched and processed in 
		// SectionProcessor::processSection().
		throw std::out_of_range ("Unknown key in RL11 section");

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
void RL11Processor::checkConsistency ()
{}

// Processing of the unit subsections is delegated to the RLProcessor.
void RL11Processor::processSubsection (iniparser::Section* subSection)
{
	rlProcessor_.processSubsection (subSection);
}

DeviceConfig RL11Processor::getConfig ()
{
	rl11ConfigPtr->common = rlProcessor_.getConfig ();
	return move (rl11ConfigPtr);
}