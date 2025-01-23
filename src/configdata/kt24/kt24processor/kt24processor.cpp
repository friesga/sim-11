#include "kt24processor.h"

#include <utility>

using std::make_unique;
using std::move;
using std::get;

KT24Processor::KT24Processor()
{
    kt24ConfigPtr = make_unique<KT24Config> ();
}

void KT24Processor::processSection (iniparser::Section* section)
{
    m9312Processor.processSection (section);
}

void KT24Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{}

void KT24Processor::checkConsistency ()
{ }

void KT24Processor::processSubsection (iniparser::Section* subSection)
{ }

DeviceConfig KT24Processor::getConfig ()
{
    kt24ConfigPtr->m9312ConfigPtr = 
        get<shared_ptr<M9312Config>> (m9312Processor.getConfig ());
    return move (kt24ConfigPtr);
}