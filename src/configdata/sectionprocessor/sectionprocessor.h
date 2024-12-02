#ifndef _SECTIONPROCESSOR_H_
#define _SECTIONPROCESSOR_H_

#include "../iniprocessor/iniparser.h"
#include "configdata/deviceconfig/deviceconfig.h"

#include <memory>
#include <limits>
#include <string>

using std::string;
using std::shared_ptr;

class SectionProcessor
{
public:
    void processSection (iniparser::Section* section);

protected:
    size_t unitNumberFromSectionName (string name, size_t maxUnits);

private:
    virtual void processValue (iniparser::Section::ValueIterator valueIterator) = 0;
    virtual void checkConsistency () = 0;
    virtual void processSubsection (iniparser::Section *subSection) = 0;
};

#endif // _SECTIONPROCESSOR_H_