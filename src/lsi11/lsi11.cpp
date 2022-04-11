#include <stdlib.h>
#include <string.h>

#include "trace.h"
#include "lsi11.h"

LSI11::LSI11 ()
{
	bus.trap = emptyIntrptReq;
	bus.delay = 0;
	bus.irq = emptyIntrptReq;
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
