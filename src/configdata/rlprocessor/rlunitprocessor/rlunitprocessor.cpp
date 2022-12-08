#include "rlunitprocessor.h"

void RlUnitProcessor::processSection (iniparser::Section* section)
{
	if (section->name().substr(0, 4) != "unit")
		throw std::invalid_argument {"Unknown RL subsection: " + 
			section->name()};

	// Get the unit number from the subsection name. This will throw an
	// exception if an incorrect unit number is specified. The unit number
	// is stored in the RlUnitConfig struct so it is clear to which unit
	// the configuration applies.
	unitNumber_ = unitNumberFromSectionName (section->name());
	
	// Process section's Values (i.e. key/value pairs)
	for (iniparser::Section::ValueIterator valueIterator = section->valuesBegin();
			valueIterator != section->valuesEnd(); ++valueIterator)
	{
		try
		{
			Process processFunction = processValue.at (valueIterator->first);
			(this->*processFunction)(valueIterator->second);
		}
		catch (std::out_of_range const &)
		{
			 throw std::invalid_argument {"Unknown key: " + 
				 valueIterator->second.asString() +
				 "in section RL: " +  valueIterator->first};
		}
	}
}

// Return the unit number to which the current configuration applies
size_t RlUnitProcessor::getUnitNumber()
{
	return unitNumber_;
}

// Copy the unit configuration to the caller to prevent a dangling
// pointer to the result
RlUnitConfig RlUnitProcessor::getConfig ()
{
	return rlUnitConfig;
}

void RlUnitProcessor::processType (iniparser::Value value)
{
	if (value.asString() == "RL01")
		rlUnitConfig.rlUnitType = RlUnitConfig::RLUnitType::RL01;
	else if (value.asString() == "RL02")
		rlUnitConfig.rlUnitType = RlUnitConfig::RLUnitType::RL02;
	else if (value.asString() == "Auto")
		rlUnitConfig.rlUnitType = RlUnitConfig::RLUnitType::Auto;
	else
		throw std::invalid_argument {"Incorrect RL unit type: " + 
			value.asString()};
}

#ifdef _WIN32
static const std::string systemType {"Windows"};
#else
static const std::string systemType {"Linux"};
#endif

// Get filename as a string from the given Value.
// The file name has to be specified as a system-independent string or
// as a map in the format "Windows:filename, Linux:filename".
void RlUnitProcessor::processFileName (iniparser::Value value)
{
	iniparser::Map aMap = value.asMap ();

	// If no system-specific filename is found in the given value map,
    // the unqualified name is returned
	rlUnitConfig.fileName = 
		aMap.getValue (iniparser::Value(systemType),
        aMap.getValue (iniparser::Value(""))).asString();
}

void RlUnitProcessor::processNewFile (iniparser::Value value)
{
	rlUnitConfig.newFile = value.asBool();
}

void RlUnitProcessor::processReadOnly (iniparser::Value value)
{
	rlUnitConfig.readOnly = value.asBool();
}

void RlUnitProcessor::processOverwrite (iniparser::Value value)
{
	rlUnitConfig.overwrite = value.asBool();
}

size_t RlUnitProcessor::unitNumberFromSectionName (std::string name)
{
	size_t unitNumber;

	try
	{
		unitNumber = stol (name.substr(4, 1));
	}
	catch (std::invalid_argument const &)
	{
		throw std::invalid_argument {"RL invalid unit number"};
	}
	
	if (unitNumber > 3)
		throw std::invalid_argument {"RL unit number out of range 0-3"};

	return unitNumber;
}

// ToDo: Check that when newFile is false, the given file exists
// ToDo: Add check that file size and unit type match
void RlUnitProcessor::checkConsistency ()
{
	// A combination of RLUnitType Auto and option newFile true is illegal.
	if (rlUnitConfig.rlUnitType == RlUnitConfig::RLUnitType::Auto &&
		rlUnitConfig.newFile)
			throw std::invalid_argument 
			{"The RLUnitType Auto is only applicable to existing files"};
}