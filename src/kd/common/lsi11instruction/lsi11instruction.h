#ifndef _LSI11INSTRUCTION_H_
#define _LSI11INSTRUCTION_H_

#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/include/mmu.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/operand/operand.h"
#include "kd/include/psw.h"
#include "kd/common/conditioncodes/conditioncodes.h"

#include <string>

using std::string;

class LSI11Instruction
{
public:
	// All derived classes have to provide a function returning the operation
	// code of the instruction. This detour to get at the instruction's
	// opcode is necessary as the decoding of the instruction takes places
	// after LSII11Instruction's constructor is called.
	virtual u16 getOperationCode () const = 0;

protected:
	CpuData* cpuData_;
	CpuControl* cpuControl_;
	MMU* mmu_;

	LSI11Instruction ();
};

#endif // _LSI11INSTRUCTION_H_