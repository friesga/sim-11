#include "kt24processor.h"

#include <utility>
#include <variant>

using std::make_unique;
using std::move;
using std::get;

KT24Processor::KT24Processor()
{}

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
    shared_ptr<M9312Config> m9312ConfigPtr = 
        std::get<shared_ptr<M9312Config>> (m9312Processor.getConfig ());
    kt24Config.m9312Config =  *m9312ConfigPtr;

    return make_shared<KT24Config> (kt24Config);
}