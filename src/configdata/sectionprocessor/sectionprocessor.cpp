#include "sectionprocessor.h"

#include <limits>

using std::to_string;

string SectionProcessor::sectionName () const
{
	return currentSection_->name ();
}

size_t SectionProcessor::unitNumberFromSectionName (string name, size_t maxUnits)
{
	size_t unitNumber;

	try
	{
		unitNumber = stol (name.substr(4, 1));
	}
	catch (std::invalid_argument const &)
	{
		throw std::invalid_argument {"Invalid unit number: " + name};
	}
	
	if (unitNumber >= maxUnits)
		throw std::invalid_argument {"Unit number out of range 0-" +
			to_string (maxUnits - 1)};

	return unitNumber;
}

void SectionProcessor::processSection (iniparser::Section* section)
{
	// Save a pointer to the section so value processors can access the
	// section's name.
	currentSection_ = section;

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