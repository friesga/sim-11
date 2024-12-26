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

void M9312Processor::processStartingAddress (iniparser::Value value)
{
    u16 startingAddress {0};
    try
    {
        startingAddress = touint<u16> (value.asString ());
    }
    catch (std::invalid_argument const&)
    {
        throw invalid_argument {"Incorrect starting address in M9312 section specified: " +
            value.asString ()};
    }

    // Check is either in the 0173000 to 0174000 or 165000 to 166000.
    if (!addressInRange (startingAddress))
        throw invalid_argument {"M9312 starting address must be in the range 0173000 to 0174000 or 0165000 to 0166000"};

    m9312ConfigPtr->startingAddress = startingAddress;
}

bool M9312Processor::addressInRange (u16 address)
{
    return (address >= 0173000 && address < 0174000) ||
           (address >= 0165000 && address < 0166000);
}