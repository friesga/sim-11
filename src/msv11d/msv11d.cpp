#include <string.h>
#include <stdlib.h>

#include "types.h"
#include "msv11d.h"

MSV11D::MSV11D ()
{
	data = (u8*) malloc (MSV11D_SIZE);
	memset (data, 0, MSV11D_SIZE);
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