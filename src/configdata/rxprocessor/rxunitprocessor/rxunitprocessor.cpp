#include "rxunitprocessor.h"

void RxUnitProcessor::processSection (iniparser::Section* section)
{
	if (section->name().substr(0, 4) != "unit")
		throw std::invalid_argument {"Unknown RX subsection: " + 
			section->name()};

	// Get the unit number from the subsection name. This will throw an
	// exception if an incorrect unit number is specified. The unit number
	// is stored in the RxUnitConfig struct so it is clear to which unit
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
				 "in section RX: " +  valueIterator->first};
		}
	}
}

// Return the unit number to which the current configuration applies
size_t RxUnitProcessor::getUnitNumber()
{
	return unitNumber_;
}

// Copy the unit configuration to the caller to prevent a dangling
// pointer to the result
RxUnitConfig RxUnitProcessor::getConfig ()
{
	return rxUnitConfig;
}


#ifdef _WIN32
static const std::string systemType {"Windows"};
#else
static const std::string systemType {"Linux"};
#endif

// Get filename as a string from the given Value.
// The file name has to be specified as a system-independent string or
// as a map in the format "Windows:filename, Linux:filename".
void RxUnitProcessor::processFileName (iniparser::Value value)
{
	iniparser::Map aMap = value.asMap ();

	// If no system-specific filename is found in the given value map,
    // the unqualified name is returned
	rxUnitConfig.fileName = 
		aMap.getValue (iniparser::Value(systemType),
        aMap.getValue (iniparser::Value(""))).asString();
}


size_t RxUnitProcessor::unitNumberFromSectionName (std::string name)
{
	size_t unitNumber;

	try
	{
		unitNumber = stol (name.substr(4, 1));
	}
	catch (std::invalid_argument const &)
	{
		throw std::invalid_argument {"RX invalid unit number"};
	}
	
	if (unitNumber > 1)
		throw std::invalid_argument {"RX unit number out of range 0-1"};

	return unitNumber;
}

// ToDo: Check that when newFile is false, the given file exists
// ToDo: Add check that file size and unit type match
void RxUnitProcessor::checkConsistency ()
{}