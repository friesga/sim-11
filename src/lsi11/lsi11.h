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
};

#endif // !_LSI11_H_
