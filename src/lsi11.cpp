#include <stdlib.h>
#include <string.h>

#include "lsi11.h"
#include "trace.h"

LSI11::LSI11 ()
{
	KD11Init (&cpu);
	bus.trap = 0;
	bus.delay = 0;
	bus.irq = 0;
}

LSI11::~LSI11 ()
{
	/* nothing */
}

void LSI11::reset ()
{
	KD11Reset(&cpu);
	bus.reset();
}

void LSI11::step ()
{
	bus.step ();
	KD11Step (&cpu, &bus);
}
