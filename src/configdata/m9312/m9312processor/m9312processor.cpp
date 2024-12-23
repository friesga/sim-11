#include "m9312processor.h"
#include "touint.h"

#include <stdexcept>
#include <vector>

using std::make_unique;
using std::move;
using std::invalid_argument;
using std::out_of_range;
using std::vector;

M9312Processor::M9312Processor ()
{
    m9312ConfigPtr = make_unique<M9312Config> ();
}

void M9312Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    Process processFunction = valueProcessors[valueIterator->first];

    if (processFunction == nullptr)
        // This exception will be catched and processed in 
        // SectionProcessor::processSection().
        throw std::out_of_range ("Unknown key in M9312 section");

    (this->*processFunction)(valueIterator->second);
}

void M9312Processor::checkConsistency ()
{}

void M9312Processor::processSubsection (iniparser::Section* subSection)
{}

DeviceConfig M9312Processor::getConfig ()
{
    return move (m9312ConfigPtr);
}

void M9312Processor::processDiagnosticROM (iniparser::Value value)
{
    try
    {
        m9312ConfigPtr->diagnosticROM = diagROMSpec.at (value.asString ());
    }
    catch (out_of_range const&)
    {
        throw invalid_argument {"Incorrect M9312 diagnostic rom: " +
            value.asString ()};
    }
}

void M9312Processor::processBootROMs (iniparser::Value value)
{
    vector<string> specifiedROMs = value.asArray ().ToVector<string> ();

    if (specifiedROMs.size () > 4)
        throw invalid_argument {"M9312 can have at most four boot ROMs"};

    for (size_t slotNr = 0; slotNr < 4 && slotNr < specifiedROMs.size (); ++slotNr)
    {
        try
        {
            m9312ConfigPtr->bootROM[slotNr] = 
                bootROMSpec.at (specifiedROMs[slotNr]);
        }
        catch (out_of_range const&)
        {
            throw invalid_argument {"Incorrect M9312 boot rom: " +
                specifiedROMs[slotNr]};
        }
    }
}

void M9312Processor::processAddressOffset (iniparser::Value value)
{
    u16 addressOffset {0};
    try
    {
        addressOffset = touint<u16> (value.asString ());
    }
    catch (std::invalid_argument const&)
    {
        throw invalid_argument {"Incorrect address offset in M9312 section specified: " +
            value.asString ()};
    }

    if (addressOffset >= 0400)
        throw invalid_argument {"The M9312 address offset maximum value is 0376"};

    if (addressOffset & 1)
        throw invalid_argument {"The M9312 address offset must be even"};

    m9312ConfigPtr->addressOffset = addressOffset;
}
