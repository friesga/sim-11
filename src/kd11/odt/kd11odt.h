#ifndef _KD11ODT_H_
#define _KD11ODT_H_

#include "../cpu/kd11cpu.h"
#include "qbus/qbus.h"
#include "types.h"

/* ODT states */
#define	ODT_STATE_INIT		0
#define	ODT_STATE_WAIT		1
#define	ODT_STATE_ADDR		2
#define	ODT_STATE_REG		3
#define	ODT_STATE_REG_WAIT	4
#define	ODT_STATE_VAL		5
#define	ODT_STATE_REG_VAL	6
#define	ODT_STATE_WR		7

// ODT functionality of the KD11
class KD11ODT 
{
public:
	KD11ODT (Qbus *bus, KD11CPU &cpu);
	void reset ();
	void step ();

private:
	void clear ();
	void write (u8 c);
	void writeOctal (u16 val);
	void inputError ();

	Qbus *bus_;
	KD11CPU &cpu_;
	u16	addr;
	u16	val;
	u8	input;
	u8	state;
	u8	next;
	u8	buf[16];
	u8	buf_r;
	u8	buf_sz;
};

#endif // _KD11ODT_H_