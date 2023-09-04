#include "kdf11_aprocessor.h"
#include "../kdf11_aconfig/kdf11_aconfig.h"

using std::make_unique;
using std::move;

KDF11_AProcessor::KDF11_AProcessor ()
{
	kdf11_aConfigPtr = make_unique<KDF11_AConfig> ();
}

void KDF11_AProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{}

// Check the consistency of the configuration of the BA11. Currently
// this is a void.
void KDF11_AProcessor::checkConsistency ()
{
}

void KDF11_AProcessor::processSubsection (iniparser::Section *subSection)
{}

unique_ptr<DeviceConfig> KDF11_AProcessor::getConfig ()
{
	return move (kdf11_aConfigPtr);
}