#include "qbus.h"

Signal &Qbus::SRUN ()
{
    return SRUN_;
}

Signal &Qbus::BPOK ()
{
    return BPOK_;
}

Signal &Qbus::RESTART ()
{
    return RESTART_;
}

Signal &Qbus::BHALT ()
{
    return BHALT_;
}

Signal &Qbus::BINIT ()
{
    return BINIT_;
}

Signal &Qbus::EXIT ()
{
    return EXIT_;
}