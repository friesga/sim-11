#include "kdf11_cpucontrol.h"
#include "instructionformats.h"

double KDF11_CpuControl::calcInstructionTime (u16 instruction)
{
	KD11Insn1* insn1 = (KD11Insn1*) &instruction;
	KD11Insn2* insn2 = (KD11Insn2*) &instruction;

	// Note that the decoding of the instructions is dependent on the 
	// order of the following switch statements.
	switch (instruction & 0177700)
	{
		case 0005000: // CLR
			[[fallthrough]];

		case 0105000: // CLRB
			[[fallthrough]];

		case 0005100: // COM
			[[fallthrough]];

		case 0105100: // COMB
			return basicB6Group1Times.getMMUEnabledTime (insn1->mode) +
				destinationB5Group1Times.getMMUEnabledTime (insn1->mode);

		case 0005200: // INC
			[[fallthrough]];

		case 0105200: // INCB
			[[fallthrough]];

		case 0005300: // DEC
			[[fallthrough]];

		case 0105300: // DECB
			[[fallthrough]];

		case 0005400: // NEG
			[[fallthrough]];

		case 0105400: // NEGB
			return basicB6Group1Times.getMMUEnabledTime (insn1->mode) +
				destinationB5Group3Times.getMMUEnabledTime (insn1->mode);

		case 0005700: // TST
			[[fallthrough]];

		case 0105700: // TSTB
			return basicB6Group1Times.getMMUEnabledTime (insn1->mode) +
				destinationB5Group2Times.getMMUEnabledTime (insn1->mode);

		case 0006200: // ASR
			[[fallthrough]];

		case 0106200: // ASRB
			[[fallthrough]];

		case 0006300: // ASL
			[[fallthrough]];

		case 0106300: // ASLB
			[[fallthrough]];

		case 0006000: // ROR
			[[fallthrough]];

		case 0106000: // RORB
			[[fallthrough]];

		case 0006100: // ROL
			[[fallthrough]];

		case 0106100: // ROLB
			[[fallthrough]];

		case 0000300: // SWAB 
			[[fallthrough]];

		case 0005500: // ADC
			[[fallthrough]];

		case 0105500: // ADCB
			[[fallthrough]];

		case 0005600: // SBC
			[[fallthrough]];

		case 0105600: // SBCB
			return basicB6Group1Times.getMMUEnabledTime (insn1->mode) +
				destinationB5Group3Times.getMMUEnabledTime (insn1->mode);

		case 0006700: // SXT
			[[fallthrough]];

		case 0106700: // MFPS
			return basicB6Group1Times.getMMUEnabledTime (insn1->mode) +
				destinationB5Group1Times.getMMUEnabledTime (insn1->mode);

		case 0106400: // MTPS
			return basicB6Group2Times.getMMUEnabledTime (insn1->mode) +
				sourceB4Group2Times.getMMUEnabledTime (insn1->mode);

		case 0006500: // MFPD
			[[fallthrough]];

		case 0106500: // MFPI
			return basicB6Group3Times.getMMUEnabledTime (insn1->mode) +
				sourceB4Group2Times.getMMUEnabledTime (insn1->mode);

		case 0006600: // MTPD 
			[[fallthrough]];

		case 0106600: // MTPI
			return basicB6Group4Times.getMMUEnabledTime (insn1->mode) +
				destinationB5Group1Times.getMMUEnabledTime (insn1->mode);

		case 0000100: // JMP
			return basicB7JMPTimes.getMMUEnabledTime (insn1->mode);

		case 0006400: // MARK
			return basicB6Group9Times.getMMUEnabledTime (insn1->mode);
	}

	switch (instruction & 0170000)
	{
		case 0010000: // MOV
			[[fallthrough]];

		case 0110000: // MOVB
			return basicB6Group1Times.getMMUEnabledTime (insn2->dst_mode) +
				sourceB4Group1Times.getMMUEnabledTime (insn2->src_mode) +
				destinationB5Group1Times.getMMUEnabledTime (insn2->dst_mode);

		case 0020000: // CMP
			[[fallthrough]];

		case 0120000: // CMPB 
			return basicB6Group1Times.getMMUEnabledTime (insn2->dst_mode) +
				sourceB4Group1Times.getMMUEnabledTime (insn2->src_mode) +
				destinationB5Group2Times.getMMUEnabledTime (insn2->dst_mode);

		case 0060000: // ADD
			[[fallthrough]];

		case 0160000: // SUB 
			return basicB6Group1Times.getMMUEnabledTime (insn2->dst_mode) +
				sourceB4Group1Times.getMMUEnabledTime (insn2->src_mode) +
				destinationB5Group3Times.getMMUEnabledTime (insn2->dst_mode);

		case 0030000: // BIT
			[[fallthrough]];

		case 0130000: // BITB 
			return basicB6Group1Times.getMMUEnabledTime (insn2->dst_mode) +
				sourceB4Group1Times.getMMUEnabledTime (insn2->src_mode) +
				destinationB5Group2Times.getMMUEnabledTime (insn2->dst_mode);

		case 0040000: // BIC
			[[fallthrough]];

		case 0140000: // BICB
			[[fallthrough]];

		case 0050000: // BIS 
			[[fallthrough]];

		case 0150000: // BISB
			return basicB6Group1Times.getMMUEnabledTime (insn2->dst_mode) +
				sourceB4Group1Times.getMMUEnabledTime (insn2->src_mode) +
				destinationB5Group3Times.getMMUEnabledTime (insn2->dst_mode);
	}

	switch (instruction & 0177000)
	{
		case 0004000: // JSR
			return basicB7JSRTimes.getMMUEnabledTime (insn1->mode);
 
		case 0070000: // MUL
			return basicB6Group15Times.getMMUEnabledTime (insn1->mode) +
				sourceB4Group2Times.getMMUEnabledTime (insn1->mode);

		case 0071000: // DIV
			return basicB6Group16Times.getMMUEnabledTime (insn1->mode) +
				sourceB4Group2Times.getMMUEnabledTime (insn1->mode);

		case 0072000: // ASH
			return basicB6Group17Times.getMMUEnabledTime (insn1->mode) +
				sourceB4Group2Times.getMMUEnabledTime (insn1->mode);

		case 0073000: // ASHC
			return basicB6Group18Times.getMMUEnabledTime (insn1->mode) +
				sourceB4Group2Times.getMMUEnabledTime (insn1->mode);

		case 0074000: // XOR
			// Appendix B Table B-6 (Basic Times) lacks an entry for the
			// XOR instruction (which would mean that an XOR in mode 0 would
			// take no time). We presume its basic time equals the time for
			// similar instructions.
			return basicB6Group1Times.getMMUEnabledTime (insn1->mode) +
				destinationB5Group3Times.getMMUEnabledTime (insn1->mode);

		case 0077000: // SOB
			return basicB6Group5Times.getMMUEnabledTime (0);
	}

	switch (instruction & 0177770)
	{
		case 0000200: // RTS
			return basicB6Group8Times.getMMUEnabledTime (0);

		// FIS instructions aren't supported on the KDF11
		// case 0075000: // FADD 
		// case 0075010: // FSUB 
		// case 0075020: // FMUL 
		// case 0075030: // FDIV 
	}

	switch (instruction & 0177400)
	{
		case 0000400: // BR
			[[fallthrough]];
		case 0001000: // BNE
			[[fallthrough]];
		case 0001400: // BEQ
			[[fallthrough]];
		case 0100000: // BPL
			[[fallthrough]];
		case 0100400: // BMI
			[[fallthrough]];
		case 0102000: // BVC
			[[fallthrough]];
		case 0102400: // BVS
			[[fallthrough]];
		case 0103000: // BCC
			[[fallthrough]];
		case 0103400: // BCS
			[[fallthrough]];
		case 0002000: // BGE
			[[fallthrough]];
		case 0002400: // BLT
			[[fallthrough]];
		case 0003000: // BGT
			[[fallthrough]];
		case 0003400: // BLE
			[[fallthrough]];
		case 0101000: // BHI
			[[fallthrough]];
		case 0101400: // BLOS
			return basicB6Group6Times.getMMUEnabledTime (0);

		case 0104000: // EMT
			[[fallthrough]];
		case 0104400: // TRAP
			return basicB6Group13Times.getMMUEnabledTime (0);
	}

	switch (instruction)
	{
		case 0000000: // HALT
			// Instruction timing for the HALT instruction is missing
			// from appendix B. As the timing of this instruction is not
			// relevant for timing purposes we assume it takes no time.
			return 0.0;

		case 0000001: // WAIT
			return basicB6Group14Times.getMMUEnabledTime (0);

		case 0000002: // RTI
			return basicB6Group10Times.getMMUEnabledTime (0);

		case 0000003: // BPT
			return basicB6Group12Times.getMMUEnabledTime (0);

		case 0000004: // IOT
			return basicB6Group12Times.getMMUEnabledTime (0);

		case 0000005: // RESET
			// Instruction timing for the RESET instruction is missing
			// from appendix B. Let's assume a standard time.
			return basicB6Group1Times.getMMUEnabledTime (0);

		case 0000006: // RTT
			return basicB6Group11Times.getMMUEnabledTime (0);

		case 0000007: // MFPT (only KDF11-A) 
			// Instruction timing for the MFPT instruction is missing
			// for Appendix B. Assume a standard time.
			return basicB6Group1Times.getMMUEnabledTime (0);

		case 0000240: // NOP
			[[fallthrough]];
		case 0000241: // CLC
			[[fallthrough]];
		case 0000242: // CLV
			[[fallthrough]];
		case 0000243: // CLVC
			[[fallthrough]];
		case 0000244: // CLZ
			[[fallthrough]];
		case 0000250: // CLN
			[[fallthrough]];
		case 0000257: // CCC
			[[fallthrough]];
		case 0000260: // NOP1
			[[fallthrough]];
		case 0000261: // SEC
			[[fallthrough]];
		case 0000262: // SEV
			[[fallthrough]];
		case 0000263: // SEVC
			[[fallthrough]];
		case 0000264: // SEZ 
			[[fallthrough]];
		case 0000270: // SEN
			[[fallthrough]];
		case 0000277: // SCC 
			return basicB6Group7Times.getMMUEnabledTime (0);
	}

	// Unused instruction will trap
	return 0.0;
}