#include "types.h"
#include "../tracerecord.h"
#include "cpusteprecord.h"

#include <string>

using std::string;
using std::to_string;

typedef struct {
	u16	rn:3;
	u16	mode:3;
	u16	opcode:10;
} KD11INSN1;

typedef struct {
	u16	dst_rn:3;
	u16	dst_mode:3;
	u16	src_rn:3;
	u16	src_mode:3;
	u16	opcode:4;
} KD11INSN2;

typedef struct {
	u16	offset:8;
	u16	opcode:8;
} KD11INSNBR;

typedef struct {
	u16	rn:3;
	u16	mode:3;
	u16	r:3;
	u16	opcode:7;
} KD11INSNJSR;

typedef struct {
	u16	rn:3;
	u16	opcode:13;
} KD11INSNRTS;

typedef struct {
	u16	offset:6;
	u16	rn:3;
	u16	opcode:7;
} KD11INSNSOB;

// Convert the given value to a string in octal representation of the value
string TraceRecord<CpuStepRecord>::octalToString (u16 val)
{
	string str {};
	char c;

	c = ((val >> 15) & 0x7) + '0';
	if (c != '0')
		str = c;

	for (int i = 0; i < 5; ++i)
	{
		c = ((val >> 12) & 7) + '0';
		val <<= 3;

		if (str.size () > 0 || c != '0')
			str += c;
	}

	if (str.size () == 0)
		str = '0';
	
	return str;
}


string TraceRecord<CpuStepRecord>::LSI11DisassemblePCOperand (u8 rn, u8 mode, const u16*& x, u16* pc)
{
	switch (mode)
	{
		case 2:
			*pc += 2;
			return "#" + octalToString (*(x++));

		case 3:
			*pc += 2;
			return "@#" + octalToString (*(x++));

		case 6:
			*pc += 2;
			return octalToString (*pc + *(x++));

		case 7:
			*pc += 2;
			return "@" + octalToString (*pc + *(x++));

		default:
			throw string ("Illegal PC operand");
	}
}

string TraceRecord<CpuStepRecord>::writeRn (u16 rn)
{
    if (rn == 6)
        return "SP";
    else if (rn == 7)
        return "PC";
    else
        return  "R" + to_string (rn);
}

string TraceRecord<CpuStepRecord>::LSI11DisassembleOperand (u8 rn, u8 mode, const u16*& x, u16* pc)
{
	if (rn == 7 && ((mode & 6) == 2 || (mode & 6) == 6))
		return LSI11DisassemblePCOperand (rn, mode, x, pc);
	
	switch (mode)
	{
		case 0:
			return writeRn (rn);

		case 1:
			return "(" + writeRn (rn) + ')';

		case 2:
			return "(" + writeRn (rn) + ")+";

		case 3:
			return "@(" + writeRn (rn) + ")+";

		case 4:
			return "-(" + writeRn (rn) + ')';

		case 5:
			return "@-(" + writeRn (rn) + ')';

		case 6:
			*pc += 2;
			return octalToString (*(x++)) + '(' + writeRn (rn) + ')';

		case 7:
			*pc += 2;
			return "@" + octalToString (*(x++)) + '(' + writeRn (rn) + ')';

		default:
			throw string ("Illegal operand");
	}
}

string TraceRecord<CpuStepRecord>::LSI11DisassembleBranch (s8 offset, u16 pc)
{
	string str {};
	s16 off = offset * 2;

	if (pc == 0xFFFF)
	{
		str = '.';
		if (offset >= 0)
		{
			str += '+';
			str += octalToString (off);
		}
		else
		{
			str += '-';
			str += octalToString (-off);
		}
	}
	else
		str = 'L' + octalToString (pc + off);

	return str;
}

// ToDo: As LSI11Disassemble is part of the TraceRecord<TraceCpu> class the 
// parameters insn and pc can be retrieved from the object.
string TraceRecord<CpuStepRecord>::LSI11Disassemble (const u16* insn, u16 pc)
{
	int pos = 0;
	u16 opcd = *insn;
	const u16* start = insn;
	KD11INSN1* insn1 = (KD11INSN1*) insn;
	KD11INSN2* insn2 = (KD11INSN2*) insn;
	KD11INSNBR* insnbr = (KD11INSNBR*) insn;
	KD11INSNJSR* insnjsr = (KD11INSNJSR*) insn;
	KD11INSNRTS* insnrts = (KD11INSNRTS*) insn;
	KD11INSNSOB* insnsob = (KD11INSNSOB*) insn;
	insn++;

	pc += 2;

	switch (opcd & 0177700)
	{
		case 0005000: /* CLR */
			return "CLR\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);
			
		case 0105000: /* CLRB */
			return "CLRB\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0005100: /* COM */
			return "COM\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0105100: /* COMB */
			return "COMB\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0005200: /* INC */
			return "INC\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0105200: /* INCB */
			return "INCB\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0005300: /* DEC */
			return "DEC\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0105300: /* DECB */
			return "DECB\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0005400: /* NEG */
			return "NEG\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0105400: /* NEGB */
			return "NEGB\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0005700: /* TST */
			return "TST\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0105700: /* TSTB */
			return "TSTB\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0006200: /* ASR */
			return "ASR\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0106200: /* ASRB */
			return "ASRB\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0006300: /* ASL */
			return "ASL\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0106300: /* ASLB */
			return "ASLB\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0006000: /* ROR */
			return "ROR\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0106000: /* RORB */
			return "RORB\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0006100: /* ROL */
			return "ROL\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0106100: /* ROLB */
			return "ROLB\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0000300: /* SWAB */
			return "SWAB\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0005500: /* ADC */
			return "ADC\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0105500: /* ADCB */
			return "ADCB\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0005600: /* SBC */
			return "SBC\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0105600: /* SBCB */
			return "SBCB\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0006700: /* SXT */
			return "SXT\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0106700: /* MFPS */
			return "MFPS\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0106400: /* MTPS */
			return "MTPS\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0106500: /* MFPI */
			return "MFPI\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0000100: /* JMP */
			return "JMP\t" + LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode, 
				insn, &pc);

		case 0006400: /* MARK */
			return "MARK\t" + octalToString (opcd & 077);

		case 0006500: /* MFPD */
			return "MFPD\t" + octalToString (opcd & 077);
	}
	string oper1, oper2;

	switch (opcd & 0170000)
	{
		case 0010000: /* MOV */
			// The operands have to be disassembled first in the correct order
			// before being assembled into a string. The string concatenation
			// occurs from left to right and unfortunately LSI11DisassembleOperand
			// is not side-effect free.
			oper1 = LSI11DisassembleOperand ((u8) insn2->src_rn, (u8) insn2->src_mode, insn, &pc);
			oper2 = LSI11DisassembleOperand ((u8) insn2->dst_rn, (u8) insn2->dst_mode, insn, &pc);
			return "MOV\t" + oper1 + ',' + oper2;

		case 0110000: /* MOVB */
			oper1 = LSI11DisassembleOperand ((u8) insn2->src_rn, (u8) insn2->src_mode, insn, &pc);
			oper2 = LSI11DisassembleOperand ((u8) insn2->dst_rn, (u8) insn2->dst_mode, insn, &pc);
			return string("MOVB\t") + oper1 + ',' + oper2;

		case 0020000: /* CMP */
			oper1 = LSI11DisassembleOperand ((u8) insn2->src_rn, (u8) insn2->src_mode, insn, &pc);
			oper2 = LSI11DisassembleOperand ((u8) insn2->dst_rn, (u8) insn2->dst_mode, insn, &pc);
			return string("CMP\t") + oper1 + ',' + oper2;

		case 0120000: /* CMPB */
			oper1 = LSI11DisassembleOperand ((u8) insn2->src_rn, (u8) insn2->src_mode, insn, &pc);
			oper2 = LSI11DisassembleOperand ((u8) insn2->dst_rn, (u8) insn2->dst_mode, insn, &pc);
			return string("CMPB\t") + oper1 + ',' + oper2;

		case 0060000: /* ADD */
			oper1 = LSI11DisassembleOperand ((u8) insn2->src_rn, (u8) insn2->src_mode, insn, &pc);
			oper2 = LSI11DisassembleOperand ((u8) insn2->dst_rn, (u8) insn2->dst_mode, insn, &pc);
			return string("ADD\t") + oper1 + ',' + oper2;

		case 0160000: /* SUB */
			oper1 = LSI11DisassembleOperand ((u8) insn2->src_rn, (u8) insn2->src_mode, insn, &pc);
			oper2 = LSI11DisassembleOperand ((u8) insn2->dst_rn, (u8) insn2->dst_mode, insn, &pc);
			return string("SUB\t") + oper1 + ',' + oper2;

		case 0030000: /* BIT */
			oper1 = LSI11DisassembleOperand ((u8) insn2->src_rn, (u8) insn2->src_mode, insn, &pc);
			oper2 = LSI11DisassembleOperand ((u8) insn2->dst_rn, (u8) insn2->dst_mode, insn, &pc);
			return string("BIT\t") + oper1 + ',' + oper2;

		case 0130000: /* BITB */
			oper1 = LSI11DisassembleOperand ((u8) insn2->src_rn, (u8) insn2->src_mode, insn, &pc);
			oper2 = LSI11DisassembleOperand ((u8) insn2->dst_rn, (u8) insn2->dst_mode, insn, &pc);
			return string("BITB\t") + oper1 + ',' + oper2;

		case 0040000: /* BIC */
			oper1 = LSI11DisassembleOperand ((u8) insn2->src_rn, (u8) insn2->src_mode, insn, &pc);
			oper2 = LSI11DisassembleOperand ((u8) insn2->dst_rn, (u8) insn2->dst_mode, insn, &pc);
			return string("BIC\t") + oper1 + ',' + oper2;

		case 0140000: /* BICB */
			oper1 = LSI11DisassembleOperand ((u8) insn2->src_rn, (u8) insn2->src_mode, insn, &pc);
			oper2 = LSI11DisassembleOperand ((u8) insn2->dst_rn, (u8) insn2->dst_mode, insn, &pc);
			return string("BICB\t") + oper1 + ',' + oper2;

		case 0050000: /* BIS */
			oper1 = LSI11DisassembleOperand ((u8) insn2->src_rn, (u8) insn2->src_mode, insn, &pc);
			oper2 = LSI11DisassembleOperand ((u8) insn2->dst_rn, (u8) insn2->dst_mode, insn, &pc);
			return string("BIS\t") + oper1 + ',' + oper2;

		case 0150000: /* BISB */
			oper1 = LSI11DisassembleOperand ((u8) insn2->src_rn, (u8) insn2->src_mode, insn, &pc);
			oper2 = LSI11DisassembleOperand ((u8) insn2->dst_rn, (u8) insn2->dst_mode, insn, &pc);
			return string("BISB\t") + oper1 + ',' + oper2;
	}

	switch (opcd & 0177000)
	{
		case 0074000: /* XOR */
			return "XOR\t" + writeRn (insnjsr->r) + ',' +
				LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode,
				insn, &pc);

		case 0004000: /* JSR */
			return "JSR\t" + writeRn (insnjsr->r) + ',' +
				LSI11DisassembleOperand ((u8) insn1->rn, (u8) insn1->mode,
				insn, &pc);

		case 0077000: /* SOB */
			return "SOB\t" + writeRn (insnjsr->r) + ",L" +
				octalToString (pc - insnsob->offset * 2);

		case 0070000: /* MUL */
			return "MUL\t" + LSI11DisassembleOperand ((u8) insn1->rn,
				(u8) insn1->mode, insn, &pc) + ',' + to_string (insnjsr->r);

		case 0071000: /* DIV */
			return "DIV\t" + LSI11DisassembleOperand ((u8) insn1->rn,
				(u8) insn1->mode, insn, &pc) + ',' + to_string (insnjsr->r);

		case 0072000: /* ASH */
			return "ASH\t" + LSI11DisassembleOperand ((u8) insn1->rn,
				(u8) insn1->mode, insn, &pc) + ',' + to_string (insnjsr->r);

		case 0073000: /* ASHC */
			return "ASHC\t" + LSI11DisassembleOperand ((u8) insn1->rn,
				(u8) insn1->mode, insn, &pc) + ',' + to_string (insnjsr->r);
	}

	switch (opcd & 0177770)
	{
		case 0000200: /* RTS */
			return "RTS\t"+ writeRn (insnrts->rn);

		case 0075000: /* FADD */
			return "FADD\t"+ writeRn (insnrts->rn);

		case 0075010: /* FSUB */
			return "FSUB\t"+ writeRn (insnrts->rn);

		case 0075020: /* FMUL */
			return "FMUL\t"+ writeRn (insnrts->rn);

		case 0075030: /* FDIV */
			return "FDIV\t"+ writeRn (insnrts->rn);
	}

	switch (opcd & 0177400)
	{
		case 0000400: /* BR */
			return "BR\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0001000: /* BNE */
			return "BNE\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0001400: /* BEQ */
			return "BEQ\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0100000: /* BPL */
			return "BPL\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0100400: /* BMI */
			return "BMI\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0102000: /* BVC */
			return "BVC\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0102400: /* BVS */
			return "BVS\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0103000: /* BCC */
			return "BCC\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0103400: /* BCS */
			return "BCS\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0002000: /* BGE */
			return "BGE\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0002400: /* BLT */
			return "BLT\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0003000: /* BGT */
			return "BGT\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0003400: /* BLE */
			return "BLE\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0101000: /* BHI */
			return "BHI\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0101400: /* BLOS */
			return "BLOS\t" + LSI11DisassembleBranch ((s8) insnbr->offset, pc);

		case 0104000: /* EMT */
			if(opcd & 0377)
				return "EMT\t" + octalToString (opcd & 0377);
			else
				return "EMT";

		case 0104400: /* TRAP */
			if(opcd & 0377)
				return "TRAP\t" + octalToString (opcd & 0377);
			else
				return "TRAP";
	}

	switch (opcd)
	{
		case 0000003: /* BPT */
			return "BPT";

		case 0000004: /* IOT */
			return "IOT";

		case 0000002: /* RTI */
			return "RTI";

		case 0000006: /* RTT */
			return "RTT";

		case 0000007: /* MFPT (only KDF11-A) */
			return "MFPT";

		case 0000000: /* HALT */
			return "HALT";

		case 0000001: /* WAIT */
			return "WAIT";

		case 0000005: /* RESET */
			return "RESET";

		case 0000240: /* NOP */
			return "NOP";

		case 0000241: /* CLC */
			return "CLC";

		case 0000242: /* CLV */
			return "CLV";

		case 0000243: /* CLVC */
			return "CLVC";

		case 0000244: /* CLZ */
			return "CLZ";

		case 0000250: /* CLN */
			return "CLN";

		case 0000257: /* CCC */
			return "CCC";

		case 0000260: /* NOP1 */
			return "NOP1";

		case 0000261: /* SEC */
			return "SEC";

		case 0000262: /* SEV */
			return "SEV";

		case 0000263: /* SEVC */
			return "SEVC";

		case 0000264: /* SEZ */
			return "SEZ";

		case 0000270: /* SEN */
			return "SEN";

		case 0000277: /* SCC */
			return "SCC";
	}

	return "; unknown [" + octalToString (opcd) + ']';
}
