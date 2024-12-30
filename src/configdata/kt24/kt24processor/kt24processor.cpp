#include "kt24processor.h"

#include <utility>

using std::make_unique;
using std::move;

KT24Processor::KT24Processor()
{
    kt24ConfigPtr = make_unique<KT24Config> ();
}

void KT24Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{ }

void KT24Processor::checkConsistency ()
{ }

void KT24Processor::processSubsection (iniparser::Section* subSection)
{ }

DeviceConfig KT24Processor::getConfig ()
{
    return move (kt24ConfigPtr);
}