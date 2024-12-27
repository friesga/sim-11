#include "qbus.h"

Signal &Qbus::SRUN ()
{
    return SRUN_;
}

Signal &Qbus::BPOK ()
{
    return BPOK_;
}

Signal &Qbus::RESET ()
{
    return RESET_;
}

Signal &Qbus::BHALT ()
{
    return BHALT_;
}

Signal &Qbus::BINIT ()
{
    return BINIT_;
}

Signal& Qbus::BOOT ()
{
    return BOOT_;
}

Signal& Qbus::BatteryPower ()
{
    return BatteryPower_;
}