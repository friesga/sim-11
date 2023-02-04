#include "sectionprocessor.h"

#include <limits>

// Return the number represented by the string as an uint16_t. The number's 
// base is determined by stoul. This allows for the specification of an
// address as an octal numer.
uint16_t SectionProcessor::touint16_t (std::string number)
{
	size_t numConvertedChars;
	uint32_t tmp = stoul (number, &numConvertedChars, 0);

	if (numConvertedChars != number.size() || 
		tmp > std::numeric_limits<uint16_t>::max())
			throw std::invalid_argument{number};

	return tmp;
}

size_t SectionProcessor::unitNumberFromSectionName (string name)
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

void SectionProcessor::processSection (iniparser::Section* section)
{
    // Process section's Values (i.e. key/value pairs)
	for (iniparser::Section::ValueIterator valueIterator = section->valuesBegin();
			valueIterator != section->valuesEnd(); ++valueIterator)
	{
		try
		{
			processValue (valueIterator);
		}
		catch (std::out_of_range const &)
		{
			throw std::invalid_argument {"Unknown key in section RL: " +  
				valueIterator->first};
		}
	}

	// Check consistency of the values in this section
	checkConsistency ();

	// Process subsections
	for (iniparser::Section* subSectionPtr : section->findSubSections())
		processSubsection (subSectionPtr);
}