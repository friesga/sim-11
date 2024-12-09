#include "m9312.h"

M9312::M9312 (Qbus* bus, shared_ptr<M9312Config> m9312Config)
    :
    PDP11Peripheral (bus)
{}

void M9312::reset ()
{}

bool M9312::responsible (BusAddress addr)
{
    return false;
}

StatusCode M9312::read (BusAddress busAddress, u16* data)
{
    return StatusCode::FunctionNotImplemented;
}

StatusCode M9312::writeByte (BusAddress busAddress, u8 data)
{
    return StatusCode::FunctionNotImplemented;
}

StatusCode M9312::writeWord (BusAddress busAddress, u16 data)
{
    return StatusCode::FunctionNotImplemented;
}