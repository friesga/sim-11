#ifndef __LSI_11_H__
#define __LSI_11_H__

#include <stdlib.h>
#include <thread>
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

class LSI11;
class QBUSMod;

class QBUS
{
public:
	QBUS ();
	int		Interrupt (int n);
	void	Reset ();
	void	Step ();
	u16		Read (u16 addr);
	void	Write (u16 addr, u16 value);
	void	InstallModule (int slot, QBUSMod* module);

	QBUSMod* slots[LSI11_SIZE];
	u16		trap;
	u16		delay;
	u16		irq;
};

class QBUSMod
{
public:
	QBUS*	bus;
	u16		virtual Read (u16 addr) = 0;
	void	virtual Write (u16 addr, u16 value) = 0;
	u8		virtual Responsible (u16 addr) = 0;
	void	virtual Reset () = 0;
	int		irq;
};

class LSI11
{
public:
	LSI11 ();
	~LSI11 ();
	void Reset ();
	void Step ();

	KD11	 cpu;
	QBUS	 bus;
};

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

/* Peripherals */
class MSV11D : public QBUSMod
{
public:
	MSV11D ();
	~MSV11D ();
	u16 Read (u16 address);
	void Write (u16 address, u16 value);
	u8 Responsible (u16 address);
	void Reset ();

// ToDo: Make data private (accessed from main())
	u8*	data;
};

class DLV11J : public QBUSMod
{
public:
	DLV11J ();
	~DLV11J ();
	u16 Read (u16 address);
	void Write (u16 address, u16 value);
	u8 Responsible (u16 address);
	void Reset ();
	void Send (int channel, unsigned char c);
	void Step();

private:
	void ReadChannel (int channelNr);
	void WriteChannel (int channelNr);
	void WriteRCSR (int n, u16 value);
	void WriteXCSR (int n, u16 value);

	DLV11Ch	channel[4];
	u16	base;
};

class BDV11 : public QBUSMod
{
public:
	BDV11 ();
	~BDV11 ();
	u16 Read (u16 address);
	void Write (u16 address, u16 value);
	u8 Responsible (u16 address);
	void Reset ();
	void Step (float dt);

private:
	u16 GetWordLow (u16 word);
	u16 GetWordHigh (u16 word);
	void MemoryDump (u16 pcr, int hi);

	u16	pcr;
	u16	scratch;
	u16	option;
	u16	display;
	u16	ltc;
	float	time;
};

class RXV21 : public QBUSMod
{
public:
	RXV21 ();
	~RXV21 ();
	u16 Read (u16 address);
	void Write (u16 address, u16 value);
	u8 Responsible (u16 address);
	void Reset ();
	void SetData (u8* data);
	void Step();

private:
	void ClearErrors ();
	void Done ();
	void FillBuffer ();
	void EmptyBuffer ();
	void WriteSector ();
	void ReadSector ();
	void ReadStatus ();
	void ReadErrorCode ();
	void ExecuteCommand ();
	void Process ();

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

class BA11_N
{
public:
	BA11_N ();

private:
	void Bezel ();

	std::thread ba11_nThread;
};

extern const u16 bdv11_e53[2048];

/* KD11 subroutines */
extern void KD11Init(KD11* kd11);
extern void KD11Reset(KD11* kd11);
extern void KD11Step(KD11* kd11, QBUS* bus);
extern void KD11Trap(KD11* kd11, int n);

/* LSI-11 disassembler */
extern int  LSI11Disassemble(const u16* insn, u16 pc, char* buf);
extern int  LSI11InstructionLength(const u16* insn);

#endif
