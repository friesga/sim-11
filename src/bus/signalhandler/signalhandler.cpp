#include "signalhandler.h"

Signal& SignalHandler::SRUN ()
{
    return SRUN_;
}

Signal& SignalHandler::BPOK ()
{
    return BPOK_;
}

Signal& SignalHandler::RESET ()
{
    return RESET_;
}

Signal& SignalHandler::BHALT ()
{
    return BHALT_;
}

Signal& SignalHandler::BINIT ()
{
    return BINIT_;
}

Signal& SignalHandler::BOOT ()
{
    return BOOT_;
}

Signal& SignalHandler::BatteryPower ()
{
    return BatteryPower_;
}

Signal& SignalHandler::IOMapEnable ()
{
    return IOMapEnable_;
}