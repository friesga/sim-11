#include "sectionprocessor.h"

#include <limits>

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
			throw std::invalid_argument {"Unknown key in section " + 
				section->name () + ": " + valueIterator->first};
		}
	}

	// Check consistency of the values in this section
	checkConsistency ();

	// Process subsections
	for (iniparser::Section* subSectionPtr : section->findSubSections())
		processSubsection (subSectionPtr);
}