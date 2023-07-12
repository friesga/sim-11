#ifndef _KD11INSNMARK_H_
#define _KD11INSNMARK_H_

// (Try to) determine the byte order. To that end gcc provides the __BYTE__ORDER__
// definition. Msvc has no such definition but we can safely assume that all
// win32 machines are little endian.
#if _WIN32 || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
// Little endian host

struct KD11INSNMARK
{
	u16	nn:6;
	u16	opcode:10;
};

#else

struct KD11INSNMARK
{
	u16	opcode:10;
	u16	nn:6;
};

#endif // _WIN32 || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

#endif // !_KD11INSNMARK_H_
