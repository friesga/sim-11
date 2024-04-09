#ifndef _INSTRUCTIONFORMATS_H_
#define _INSTRUCTIONFORMATS_H_

#include <cstdint>

using u16 = uint16_t;

// Instruction format for single operand instructions. The mode is defined
// as 'dst_mode' so calInstrTimeMMUEnabled() will always reference a mode.
struct KD11Insn1
{
	u16	rn:3;
	u16	mode:3;
	u16	opcode:10;
};

struct KD11Insn2
{
	u16	dst_rn:3;
	u16	dst_mode:3;
	u16	src_rn:3;
	u16	src_mode:3;
	u16	opcode:4;
};

struct KD11InsnBR
{
	u16	offset:8;
	u16	opcode:8;
};

struct KD11InsnJSR
{
	u16	rn:3;
	u16	mode:3;
	u16	r:3;
	u16	opcode:7;
};

struct KD11InsnRTS
{
	u16	rn:3;
	u16	opcode:13;
};

struct KD11InsnSOB
{
	u16	offset:6;
	u16	rn:3;
	u16	opcode:7;
};

#endif // _INSTRUCTIONFORMATS_H_