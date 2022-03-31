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

u16 MSV11D::read (u16 address)
{
	u16* mem = (u16*) &data[address];
	return *mem;
}

void MSV11D::write (u16 address, u16 value)
{
	u16* mem = (u16*) &data[address];
	*mem = value;
}

u8 MSV11D::responsible (u16 address)
{
	return address < MSV11D_SIZE;
}

void MSV11D::reset ()
{
	/* nothing */
}