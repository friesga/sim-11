#include <stdlib.h>
#include <string.h>

#include "lsi11.h"
#include "trace.h"

LSI11::LSI11 ()
{
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
	kd11.reset();
	bus.reset();
}

void LSI11::step ()
{
	bus.step ();
	kd11.step (&bus);
}
