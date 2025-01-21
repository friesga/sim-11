#ifndef _RECORDHEADER_H_
#define _RECORDHEADER_H_

#include "tracerecord.h"

#include <typeinfo>

// Definition of the magic's, identifying the trace record type
// in the trace file. The magic value is the hexadecimal representation
// of the enum ASCII characters (see e.g. 
// https://www.utilities-online.info/ascii-to-hex)
//
// The constants are defined as conventional enums to be able to convert them
// to byte-swapped integers in trace records.
enum class Magic
{
	VOID = 0x0,
	CPU0 = 0x43505530,
	CPUZ = 0x4350555a,
	CPU1 = 0x43505531,
	BUS0 = 0x42555330,
	BUS1 = 0x42555331,
	TRAP = 0x54524150,
	IRQ0 = 0x49525130,
	RX2A = 0x52583241,
	RX2C = 0x52583243,
	RX2D = 0x52583244,
	RX2E = 0x52583245,
	RX2S = 0x52583253,
	DLV1 = 0x444C5631,
	RL2A = 0x524C3241,	// RLV12 register
	RL2C = 0x524C3243,	// RLV12 command
	CLCK = 0x434c434b,
	MMU0 = 0x4D4D5530,
	MMUA = 0x4D4D5541,	// MMU mapped address
	UBM0 = 0x55424D30	// Unibus Map
};

// Definition of the type to discriminate the trace records in the template
// classes.
struct RecordHeader {};

// Specialization of the TraceRecord for the RecordHeader
template <>
class TraceRecord<RecordHeader>
{
	// The magic defines the trace record type.
	Magic magic_;

public:
	TraceRecord ();
	TraceRecord (Magic magic);
	Magic magic () { return magic_; }
};

inline TraceRecord<RecordHeader>::TraceRecord ()
	:
	magic_ {Magic::VOID}
{}

inline TraceRecord<RecordHeader>::TraceRecord (Magic magic)
	:
	magic_ {magic}
{}


#endif // _RECORDHEADER_H_