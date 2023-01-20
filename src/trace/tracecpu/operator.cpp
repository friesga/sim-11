#include "tracecpu.h"

std::ostream& operator<< (std::ostream& os, TraceRecord<TraceCpu> record)
{
	char buf[64];
    record.LSI11Disassemble (record.insn_, record.r_[7], buf);

	os << oct << setw(6) << setfill('0') << "[CPU ] "
		"PC=" << record.r_[7] << ' ' <<
		"PSW [" << record.priorityBit () <<
				   record.traceBit ()    <<
				   record.negativeBit () <<
				   record.zeroBit ()     <<
				   record.overflowBit () <<
		           record.carryBit ()    << "] " <<
		"SP=" << record.r_[6] << ' ' <<
		"[R0=" << record.r_[0] << ' ' <<
		"R1=" << record.r_[1] << ' ' <<
		"R2=" << record.r_[2] << ' ' <<
		"R3=" << record.r_[3] << ' ' <<
		"R4=" << record.r_[4] << ' ' <<
		"R5=" << record.r_[5] << "] " <<
		*record.insn_ << " => " << buf << '\n';

	return os;
}