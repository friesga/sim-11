#include <stdlib.h>
#include <string.h>

#include "lsi11.h"
#include "trace.h"

LSI11::LSI11 ()
{
	KD11Init (&cpu);
	bus.trap = 0;
}

LSI11::~LSI11 ()
{
	/* nothing */
}

void LSI11::Reset ()
{
	KD11Reset(&cpu);
	bus.Reset();
}

void LSI11::Step ()
{
	bus.Step ();
	KD11Step (&cpu, &bus);
}
