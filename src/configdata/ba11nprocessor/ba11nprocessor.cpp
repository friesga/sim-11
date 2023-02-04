#include "ba11nprocessor.h"
#include "../ba11nconfig/ba11nconfig.h"

using std::make_unique;
using std::move;

BA11_NProcessor::BA11_NProcessor ()
{
	ba11_nConfigPtr = make_unique<BA11_NConfig> ();
}

void BA11_NProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{}

// Check the consistency of the configuration of the BA11. Currently
// this is a void.
void BA11_NProcessor::checkConsistency ()
{
}

void BA11_NProcessor::processSubsection (iniparser::Section *subSection)
{}

unique_ptr<DeviceConfig> BA11_NProcessor::getConfig ()
{
	return move (ba11_nConfigPtr);
}