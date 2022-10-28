#include <stdlib.h>
#include <string.h>

#include "trace/trace.h"
#include "lsi11.h"

LSI11::LSI11 ()
{
	// ToDo: Initialization of bus.delay belongs in class QBUS
	bus.delay = 0;
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
