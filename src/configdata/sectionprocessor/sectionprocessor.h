#ifndef _SECTIONPROCESSOR_H_
#define _SECTIONPROCESSOR_H_

#include "../iniprocessor/iniparser.h"
#include "../deviceconfig/deviceconfig.h"

#include <memory>

using std::string;
using std::unique_ptr;

class SectionProcessor
{
    virtual void processValue (iniparser::Section::ValueIterator valueIterator) = 0;
    virtual void checkConsistency () = 0;
    virtual void processSubsection (iniparser::Section *subSection) = 0;

protected:
    uint16_t touint16_t (string number);
    size_t unitNumberFromSectionName (string name);

public:
    void processSection (iniparser::Section* section);
    virtual unique_ptr<DeviceConfig> getConfig () = 0;
};

#endif // _SECTIONPROCESSOR_H_