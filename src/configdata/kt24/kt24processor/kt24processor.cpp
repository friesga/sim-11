#include "kt24processor.h"

#include <utility>
#include <variant>

using std::make_unique;
using std::move;
using std::get;

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
    kt24Config.m9312Config = 
        get<M9312Config> (m9312Processor.getConfig ());

    return kt24Config;
}