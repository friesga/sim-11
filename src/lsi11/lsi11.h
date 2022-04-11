#ifndef _LSI11_H_
#define _LSI11_H_

#include "../kd11/kd11.h"
#include "../qbus/qbus.h"

class LSI11
{
public:
	LSI11 ();
	~LSI11 ();
	void reset ();
	void step ();

	KD11	 kd11;
	QBUS	 bus;

private:
	InterruptRequest const emptyIntrptReq{TrapPriority::None, 0, 0};
};

#endif // !_LSI11_H_
