#ifndef _KD11INSNSOB_H_
#define _KD11INSNSOB_H_

// (Try to) determine the byte order. To that end gcc provides the __BYTE__ORDER__
// definition. Msvc has no such definition but we can safely assume that all
// win32 machines are little endian.
#if _WIN32 || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
// Little endian host

struct KD11INSNSOB
{
	u16	offset:6;
	u16	rn:3;
	u16	opcode:7;
};
#else

struct KD11INSNSOB
{
	u16	opcode:7;
	u16	rn:3;
	u16	offset:6;
};
#endif // _WIN32 || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

#endif // !_KD11INSNSOB_H_
