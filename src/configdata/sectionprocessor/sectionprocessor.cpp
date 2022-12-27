#include "sectionprocessor.h"

#include <limits>

// Return the number represented by the string as an uint16_t. The number's 
// base is determined by stoul. This allows for the specification of an
// address as an octal numer.
uint16_t SectionProcessor::touint16_t (std::string number)
{
	size_t numConvertedChars;
	uint32_t tmp = stoul (number, &numConvertedChars, 0);

	if (numConvertedChars != number.size() || 
		tmp > std::numeric_limits<uint16_t>::max())
			throw std::invalid_argument{number};

	return tmp;
}