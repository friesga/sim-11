#include "sectionprocessor.h"

#include <limits>

using std::to_string;

string SectionProcessor::sectionName () const
{
	return currentSection_->name ();
}

// After use in this function, the unit number is removed from the section
// to avoid it being processed again by the SectionProcessor base class.
size_t SectionProcessor::unitNumberFromUnitKey (iniparser::Section* subSection,
	size_t maxUnits) const
{
	size_t unitNumber;

	// Get the unit number from the unit key in this subsection.
	iniparser::Value unitValue = subSection->getValue ("unit", 0);

	try
	{
		unitNumber = unitValue.asInt ();
	}
	catch (std::invalid_argument const&)
	{
		throw std::invalid_argument {"Invalid unit number in section " +
			subSection->fullName ()};
	}

	// Check validity of the unit number
	if (unitNumber >= maxUnits)
		throw std::invalid_argument {"Unit number out of range 0-" +
			to_string (maxUnits - 1) + " in section " + subSection->fullName ()};

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