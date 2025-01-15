#include "qbus.h"

Signal& Qbus::SRUN ()
{
    return signalHandler_.SRUN ();
}

Signal& Qbus::BPOK ()
{
    return signalHandler_.BPOK ();
}

Signal& Qbus::RESET ()
{
    return signalHandler_.RESET ();
}

Signal& Qbus::BHALT ()
{
    return signalHandler_.BHALT ();
}

Signal& Qbus::BINIT ()
{
    return signalHandler_.BINIT ();
}

Signal& Qbus::BOOT ()
{
    return signalHandler_.BOOT ();
}

Signal& Qbus::BatteryPower ()
{
    return signalHandler_.BatteryPower ();
}

Signal& Qbus::IOMapEnable ()
{
    return signalHandler_.IOMapEnable ();
}