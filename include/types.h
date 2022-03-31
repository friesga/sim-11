#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

#define	u8	uint8_t
#define	s8	int8_t
#define	u16	uint16_t
#define	s16	int16_t
#define	u32	uint32_t
#define	s32	int32_t
#define	u64	uint64_t
#define	s64	int64_t

#define	_BV(x)	(1 << (x))

#ifdef WIN32
// We can safely asseume that all Win32 systems are little endian
#define	U16B(x)			_byteswap_ushort(x)
#define	U32B(x)			_byteswap_ulong(x)
#define	U64B(x)			_byteswap_uint64(x)
#define	U16L(x)			(x)
#define	U32L(x)			(x)
#define	U64L(x)			(x)
#else // WIN32
// Use byte order as defined by gcc
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define	U16B(x)			(x)
#define	U32B(x)			(x)
#define	U64B(x)			(x)
#define	U16L(x)			__builtin_bswap16(x)
#define	U32L(x)			__builtin_bswap32(x)
#define	U64L(x)			__builtin_bswap64(x)
#else	// __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define	U16B(x)			__builtin_bswap16(x)
#define	U32B(x)			__builtin_bswap32(x)
#define	U64B(x)			__builtin_bswap64(x)
#define	U16L(x)			(x)
#define	U32L(x)			(x)
#define	U64L(x)			(x)
#endif	// __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#endif // WIN32

#endif
