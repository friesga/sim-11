#ifndef _KW11LPROCESSOR_H_
#define _KW11LPROCESSOR_H_

#include "configdata/sectionprocessor/deviceconfigprocessor.h"

#include <map>

using std::map;

class KW11LProcessor : public DeviceConfigProcessor
{
public:
    DeviceConfig getConfig ();

private:
    KW11LConfig kw11lConfig_;

    // Define process as a pointer to a KW11LProcessor member function
    // with a iniparser::Value argument and returning void.
    typedef void (KW11LProcessor::* Process)(iniparser::Value);

    map<string, Process> valueProcessors =
    {
        {"clock_source", &KW11LProcessor::processClockSource}
    };

    map<string, KW11LConfig::ClockSource> clockSourceSpec =
    {
        {"simulator_clock", KW11LConfig::ClockSource::SimulatorClock},
        {"system_clock",    KW11LConfig::ClockSource::SystemClock}
    };

    // Definition of functions to satisfy the pure virtual functions
    // of the base class.
    void processValue (iniparser::Section::ValueIterator valueIterator);
    void checkConsistency () {}
    void processSubsection (iniparser::Section* subSection) {}
    
    void processClockSource (iniparser::Value value);
};


#endif // _KW11LPROCESSOR_H_