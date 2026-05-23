#ifndef _KW11LPROCESSOR_H_
#define _KW11LPROCESSOR_H_

#include "configdata/sectionprocessor/deviceconfigprocessor.h"

class KW11LProcessor : public DeviceConfigProcessor
{
public:
    DeviceConfig getConfig ();

private:
    // Definition of dummy functions to satisfy the pure virtual functions
    // of the base class. The KW11-L has no configurable options.
    void processValue (iniparser::Section::ValueIterator valueIterator) {}
    void checkConsistency () {}
    void processSubsection (iniparser::Section* subSection) {}
};


#endif // _KW11LPROCESSOR_H_