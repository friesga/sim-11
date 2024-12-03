#include "rlv12processor.h"
#include "configdata/rl/rlunitconfig/rlunitconfig.h"
#include "configdata/rl/rlunitprocessor/rlunitprocessor.h"
#include "touint.h"

#include <utility>

using std::make_unique;
using std::move;

RLV12Processor::RLV12Processor ()
{
	rlConfigPtr = make_unique<RLV12Config> ();
}

// This is a RLV12-specific processSection(). This version first calls the
// RL configuration processor to process the common keys and then calls the
// regular ProcessSection() to process the remaining keys. Common keys are
// deleted from the section so they aren't processed by the regular
// ProcessSection().
void RLV12Processor::processSection (iniparser::Section* section)
{
	rlProcessor_.processSection (section);
	
	SectionProcessor::processSection (section);
}

void RLV12Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    Process processFunction = valueProcessors[valueIterator->first];
	if (processFunction == nullptr)
		// This exception will be catched and processed in 
		// SectionProcessor::processSection().
		throw std::out_of_range ("Unknown key in RLV12 section");

    (this->*processFunction)(valueIterator->second);
}

// Explicitly test for "true" and "false" as AsBool() returns no error,
// only checks the first character and returns false except for strings parsed
// as true.
void RLV12Processor::process22Bit (iniparser::Value value)
{
	if (value.asString() == "true")
		rlConfigPtr->_22bit = true;
	else if (value.asString() == "false")
		rlConfigPtr->_22bit = false;
	else
		throw std::invalid_argument {"22-bit must be either true or false"};
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
void RLV12Processor::checkConsistency ()
{
	if (rlConfigPtr->common.rlType == RLConfig::RLType::RL11)
		throw std::invalid_argument 
			{"The RL11 can only be configured on Unibus systems"};

	if (rlConfigPtr->_22bit && rlConfigPtr->common.rlType != RLConfig::RLType::RLV12)
		throw std::invalid_argument 
			{"The 22-bit option is only allowed on an RLV12 controller"};
}

// Processing of the unit subsections is delegated to the RLProcessor.
void RLV12Processor::processSubsection (iniparser::Section *subSection)
{
	rlProcessor_.processSubsection (subSection);
}

DeviceConfig RLV12Processor::getConfig ()
{
	rlConfigPtr->common = rlProcessor_.getConfig ();
	return move (rlConfigPtr);
}