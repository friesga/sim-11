#ifndef _TOUINT_H_
#define _TOUINT_H_

// Return the number represented by the string as the specified unsigned
// integer type (typically u16 or u32). The number's base is determined by
// stoul. This allows for the specification of an address as an octal numer.
template <typename T>
T touint (string number)
{
	size_t numConvertedChars;
	uint64_t tmp = stoull (number, &numConvertedChars, 0);

	if (numConvertedChars != number.size () ||
		tmp > std::numeric_limits<T>::max ())
		throw std::invalid_argument {number};

	return tmp;
}

#endif // _TOUINT_H_