#include "iniprocessor.h"

using std::invalid_argument;

// This function is the entry point for the processing of the configuration
// file. It gets a vector with the data on the top level sections in
// the configuration file and then processes these sections.
void IniProcessor::process (string filename)
{
	iniparser::File iniFile;

	if (!iniFile.load (filename))
		throw invalid_argument ("Error: cannot open file " + string (filename));

	process (iniFile);
}

// Definition of an overloaded process function to be used in unit tests
void IniProcessor::process (iniparser::File iniFile)
{
	iniparser::SectionVector topLevelSections = 
		iniFile.getTopLevelSections ();

	for (iniparser::Section* sectionPtr : topLevelSections)
		processSection (sectionPtr);
}