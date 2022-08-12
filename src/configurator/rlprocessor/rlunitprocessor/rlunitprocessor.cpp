#include "rlunitprocessor.h"

// #include <iostream>

void RlUnitProcessor::processSection (iniparser::Section* section)
{
	if (section->name().substr(0, 4) != "unit")
		throw std::invalid_argument {"Unknown RL subsection: " + 
			section->name()};

	size_t unitNumber = unitNumberFromSectionName (section->name());
	
	// std::cout << "Unit number: "  << unitNumber;

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
			 throw std::invalid_argument {"Unknown key: " + 
				 valueIterator->second.asString() +
				 "in section RL: " +  valueIterator->first};
		}
	}
}

void RlUnitProcessor::processType (iniparser::Value value)
{
    // std::cout << "type: " << value.asString() << '\n';
	if (value.asString() == "RL01")
		RL01 = true;
	else if (value.asString() == "RL02")
		RL01 = false;
	else
		throw std::invalid_argument {"Incorrect RL unit type: " + 
			value.asString()};
}

void RlUnitProcessor::processFileName (iniparser::Value value)
{
	// std::cout << "filename: " << value.asString() << '\n';
	fileName = value.asString();
}

void RlUnitProcessor::processNewFile (iniparser::Value value)
{
	// std::cout << "newfile: " << value.asBool() << '\n';
	newFile = value.asBool();
}

void RlUnitProcessor::processReadOnly (iniparser::Value value)
{
	// std::cout << "read-only: " << value.asBool() << '\n';
	readOnly = value.asBool();
}

size_t RlUnitProcessor::unitNumberFromSectionName (std::string name)
{
	size_t unitNumber;

	try
	{
		unitNumber = stol (name.substr(4, 1));
	}
	catch (std::invalid_argument)
	{
		throw std::invalid_argument {"RL invalid unit number"};
	}
	
	if (unitNumber > 3)
		throw std::invalid_argument {"RL unit number out of range 0-3"};

	return unitNumber;
}