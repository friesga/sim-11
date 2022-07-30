#ifndef _MSV11D_H_
#define _MSV11D_H_

#include "qbus/qbus.h"
#include "busdevice/busdevice.h"

/* Main memory size: 32kW / 64kB */
#define	MSV11D_SIZE		(65536 - 2 * 4096)

class MSV11D : public BusDevice
{
public:
	MSV11D ();
	~MSV11D ();
	StatusCode read (u16 address, u16 *destAddress) override;
	StatusCode writeWord (u16 address, u16 value) override;
	bool responsible (u16 address) override;
	void reset ();

// ToDo: Make data private (accessed from main())
	u8*	data;
};

#endif // _MSV11D_H_