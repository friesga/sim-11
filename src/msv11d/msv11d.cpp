#include "types.h"
#include "msv11d.h"

#include <string.h>
#include <stdlib.h>

using std::bind;
using std::placeholders::_1;

MSV11D::MSV11D (Qbus *bus)
	:
	BusDevice (bus)
{
	data = (u8*) malloc (MSV11D_SIZE);
	if (data != nullptr)
		memset (data, 0, MSV11D_SIZE);

	// Subscribe to the BPOK signal
	bus_->BPOK().subscribe (bind (&MSV11D::BPOKReceiver, this, _1));
}

MSV11D::~MSV11D ()
{
	free (data);
}

StatusCode MSV11D::read (u16 address, u16 *destAddress)
{
	// Get the contents of the specified address via a u16 pointer
	// as data is an array of bytes.
	u16* mem = (u16*) &data[address];
	*destAddress = *mem;
	return StatusCode::OK;
}

StatusCode MSV11D::writeWord (u16 address, u16 value)
{
	u16* mem = (u16*) &data[address];
	*mem = value;

	return StatusCode::OK;
}

bool MSV11D::responsible (u16 address)
{
	return (address < MSV11D_SIZE) ? true : false;
}

void MSV11D::reset ()
{
	/* nothing */
}

// When the system is powered down the contents of the memory are lost, unless
// a battery backup is available.
// The memory is initialized when the power is switched on.
void MSV11D::BPOKReceiver (bool signalValue)
{
	if (signalValue)
		memset (data, 0, MSV11D_SIZE);
}