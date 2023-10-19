#ifndef _OPERAND_H_
#define _OPERAND_H_

#include "types.h"

struct Operand
{
	Operand (u16 registerNr, u16 mode);

	u16	registerNr_;
	u16	mode_;
};

inline Operand::Operand (u16 registerNr, u16 mode)
	:
	registerNr_ {registerNr},
	mode_ {mode}
{}


#endif // _OPERAND_H_