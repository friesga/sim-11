#ifndef _MSV11D_H_
#define _MSV11D_H_

#include "qbus/qbus.h"

/* Main memory size: 32kW / 64kB */
#define	MSV11D_SIZE		(65536 - 2 * 4096)

class MSV11D : public QBUSModule
{
public:
	MSV11D ();
	~MSV11D ();
	StatusCode read (u16 address, u16 *destAddress) override;
	void write (u16 address, u16 value);
	u8 responsible (u16 address);
	void reset ();

// ToDo: Make data private (accessed from main())
	u8*	data;
};

#endif // _MSV11D_H_