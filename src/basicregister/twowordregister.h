#ifndef _TWOWORDREGISTER_H_
#define _TWOWORDREGISTER_H_

#include "types.h"

struct TwoWordRegister
{
	u16 low;
	u16 high;

	TwoWordRegister& operator= (u32 const value)
	{
		low = value & 0177777;
		high = value >> 16;

		return *this;
	}
};

#endif // _TWOWORDREGISTER_H_