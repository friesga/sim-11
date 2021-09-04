#ifndef __LSI_11_H__
#define __LSI_11_H__

#include <stdlib.h>
#include "types.h"

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

/* Main memory size: 32kW / 64kB */
#define	MSV11D_SIZE		(65536 - 2 * 4096)

/* DLV11-J input buffer */
#define	DLV11J_BUF		2048

/* Backplane size */
#define	LSI11_SIZE		8

/* QBUS interrupt request delay */
#define	QBUS_DELAY		20

/* QBUS interrupt request delay jitter */
#define	QBUS_DELAY_JITTER	10

/* LTC rate */
#define	LTC_RATE		50
#define	LTC_TIME		(1.0F / LTC_RATE)

typedef struct {
	u16	addr;
	u16	val;
	u8	input;
	u8	state;
	u8	next;
	u8	buf[16];
	u8	buf_r;
	u8	buf_sz;
} KD11ODT;

typedef struct {
	u16	r[8];
	u16	psw;
	KD11ODT	odt;
	u8	state;
	u16	trap;
} KD11;

typedef struct QBUS QBUS;
struct QBUS {
	void*	user;
	u16	trap;
	u16	delay;
	u16	irq;
	int	(*interrupt)(QBUS* self, int n);
	void	(*reset)(QBUS* self);
	u16	(*read)(void* user, u16 addr);
	void	(*write)(void* user, u16 addr, u16 value);
};

typedef struct {
	QBUS*	bus;
	void*	self;
	u16	(*read)(void* self, u16 addr);
	void	(*write)(void* self, u16 addr, u16 value);
	u8	(*responsible)(void* self, u16 addr);
	void	(*reset)(void* self);
} QBUSMod;

typedef struct {
	KD11	cpu;
	QBUS	bus;
	QBUSMod*backplane[LSI11_SIZE];
} LSI11;

typedef struct {
	u16	rcsr;
	u16	rbuf;
	u16	xcsr;
	u16	xbuf;

	u16	base;
	u16	vector;

	u8*	buf;
	u16	buf_r;
	u16	buf_w;
	u16	buf_size;
	void	(*receive)(unsigned char c);
} DLV11Ch;

/* peripherals */
struct MSV11D : QBUSMod
{
	u8*	data;
};

struct DLV11J : QBUSMod
{
	DLV11Ch	channel[4];
	u16	base;
};

struct BDV11 : QBUSMod
{
	u16	pcr;
	u16	scratch;
	u16	option;
	u16	display;
	u16	ltc;
	float	time;
};

struct RXV21 : QBUSMod
{
	u16	base;
	u16	vector;

	u16	rx2cs;
	u16	rx2db;

	u16	rx2ta;	/* RX Track Address */
	u16	rx2sa;	/* RX Sector Address */
	u16	rx2wc;	/* RX Word Count Register */
	u16	rx2ba;	/* RX Bus Address Register */
	u16	rx2es;	/* RX Error and Status */

	u16	state;
	u16	error;

	u16	buffer[128];
	u8* data;
};

typedef struct MSV11D MSV11D;
typedef struct DLV11J DLV11J;
typedef struct BDV11 BDV11;
typedef struct RXV21 RXV21;

extern const u16 bdv11_e53[2048];

extern void MSV11DInit(MSV11D* msv);
extern void MSV11DDestroy(MSV11D* msv);

extern void DLV11JInit(DLV11J* dlv);
extern void DLV11JDestroy(DLV11J* dlv);
extern void DLV11JSend(DLV11J* dlv, int channel, unsigned char c);

extern void BDV11Init(BDV11* bdv);
extern void BDV11Destroy(BDV11* bdv);
extern void BDV11Step(BDV11* bdv, float dt);

extern void RXV21Init(RXV21* rx);
extern void RXV21Destroy(RXV21* rx);
extern void RXV21SetData(RXV21* rx, u8* data);

/* KD11 subroutines */
extern void KD11Init(KD11* kd11);
extern void KD11Reset(KD11* kd11);
extern void KD11Step(KD11* kd11, QBUS* bus);
extern void KD11Trap(KD11* kd11, int n);

/* LSI-11 subroutines */
extern void LSI11Init(LSI11* lsi);
extern void LSI11Destroy(LSI11* lsi);
extern void LSI11InstallModule(LSI11* lsi, int slot, QBUSMod* module);
extern void LSI11Reset(LSI11* lsi);
extern void LSI11Step(LSI11* lsi);

/* LSI-11 disassembler */
extern int  LSI11Disassemble(const u16* insn, u16 pc, char* buf);
extern int  LSI11InstructionLength(const u16* insn);

#endif
