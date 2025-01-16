#include "unibus.h"

Signal& Unibus::SRUN ()
{
    return signalHandler_.SRUN ();
}

Signal& Unibus::BPOK ()
{
    return signalHandler_.BPOK ();
}

Signal& Unibus::RESET ()
{
    return signalHandler_.RESET ();
}

Signal& Unibus::BHALT ()
{
    return signalHandler_.BHALT ();
}

Signal& Unibus::BINIT ()
{
    return signalHandler_.BINIT ();
}

Signal& Unibus::BOOT ()
{
    return signalHandler_.BOOT ();
}

Signal& Unibus::BatteryPower ()
{
    return signalHandler_.BatteryPower ();
}

Signal& Unibus::IOMapEnable ()
{
    return signalHandler_.IOMapEnable ();
}