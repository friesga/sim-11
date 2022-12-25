#ifndef _SECTIONROCESSOR_H_
#define _SECTIONROCESSOR_H_

#include "types.h"
#include "../iniparser.h"

#include <string>

class SectionProcessor
{
protected:
	uint16_t touint16_t (std::string number);

public:
	virtual void processSection (iniparser::Section* section) = 0;
};

#endif // !_SECTIONPROCESSOR_H_
