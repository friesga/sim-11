#include "rlprocessor.h"
#include "rlunitprocessor/rlunitprocessor.h"

#include <limits>
// #include <iostream>

// Process the given RL section
void RlProcessor::processSection (iniparser::Section* section)
{
	// Process section's Values (i.e. key/value pairs)
	for (iniparser::Section::ValueIterator valueIterator = section->valuesBegin();
			valueIterator != section->valuesEnd(); ++valueIterator)
	{
		try
		{
			Process processFunction = processValue.at (valueIterator->first);
			(this->*processFunction)(valueIterator->second);
		}
		catch (std::out_of_range)
		{
			throw std::invalid_argument {"Unknown key in section RL: " +  
				valueIterator->first};
		}
	}

	// Process subsections
	// A subsection name must start with the string "unit", followed by
	// a unit number in the range 0-4.
	for (iniparser::Section* subSectionPtr : section->findSubSections())
	{
		RlUnitProcessor rlUnitProcessor;

		// std::cout << "Sub level section: " << subSectionPtr->name() << '\n';
		rlUnitProcessor.processSection (subSectionPtr);
	}
}

RlConfig *RlProcessor::getConfig()
{
	return rlConfigPtr.get();
}

void RlProcessor::processController (iniparser::Value value)
{
	// std::cout << "controller: " << value.asString() << '\n';
	if (value.asString() == "RLV11")
		rlConfigPtr->RLV11 = true;
	else if (value.asString() == "RLV12")
		rlConfigPtr->RLV11 = false;
	else
		throw std::invalid_argument {"Incorrect RL controller type: " + 
			value.asString()};
}

void RlProcessor::processAddress (iniparser::Value value)
{
	try
	{
		rlConfigPtr->address = touint16_t (value.asString());
	}
	catch (std::invalid_argument)
	{
		throw std::invalid_argument {"Incorrect address in RL section specified: " + 
			value.asString()};
	}
	// std::cout << "address: " << std::oct << address << '\n';
}

void RlProcessor::processVector (iniparser::Value value) 
{ 
	try
	{
		rlConfigPtr->vector = touint16_t (value.asString());
	}
	catch (std::invalid_argument)
	{
		throw std::invalid_argument {"Incorrect vector in RL section specified: " + 
			value.asString()};
	}
	// std::cout << "vector: " << std::oct << vector << '\n';
}

void RlProcessor::processUnits (iniparser::Value value)
{
	rlConfigPtr->numUnits = value.asInt ();
}

// Return the number represented by the string as an uint16_t. The number's 
// base is determined by stoul. This allows for the specification of an
// address as an octal numer.
uint16_t RlProcessor::touint16_t (std::string number)
{
	size_t numConvertedChars;
	uint32_t tmp = stoul (number, &numConvertedChars, 0);

	if (numConvertedChars != number.size() || 
		tmp > std::numeric_limits<uint16_t>::max())
			throw std::invalid_argument{number};

	return tmp;
}
