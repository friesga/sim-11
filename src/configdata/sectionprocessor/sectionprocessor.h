#ifndef _SECTIONPROCESSOR_H_
#define _SECTIONPROCESSOR_H_

#include "../iniprocessor/iniparser.h"
#include "../deviceconfig/deviceconfig.h"
#include "configdata/deviceconfig/deviceconfigvariant.h"

#include <memory>
#include <limits>
#include <string>

using std::string;
using std::shared_ptr;

class SectionProcessor
{
public:
    void processSection (iniparser::Section* section);
    virtual DeviceConfigVariant getConfig () = 0;

protected:
    template <typename T> T touint (string number);
    size_t unitNumberFromSectionName (string name);

private:
    virtual void processValue (iniparser::Section::ValueIterator valueIterator) = 0;
    virtual void checkConsistency () = 0;
    virtual void processSubsection (iniparser::Section *subSection) = 0;
};

// Return the number represented by the string as the specified unsigned
// integer type (typically u16 or u32). The number's base is determined by
// stoul. This allows for the specification of an address as an octal numer.
template <typename T>
T SectionProcessor::touint (string number)
{
	size_t numConvertedChars;
	uint64_t tmp = stoull (number, &numConvertedChars, 0);

	if (numConvertedChars != number.size() || 
		tmp > std::numeric_limits<T>::max())
			throw std::invalid_argument{number};

	return tmp;
}

#endif // _SECTIONPROCESSOR_H_