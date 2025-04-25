#include "rk11d.h"

StatusCode RK11D::writeWord (BusAddress busAddress, u16 value)
{
    // Guard against controller register access from the command processor
    std::unique_lock<std::mutex> lock {controllerMutex_};

    // Decode registerAddress<3:1>
    switch (busAddress.registerAddress () & 016)
    {
        case RKDS:
            // Drive Status register
            return StatusCode::Success;
            break;

        case RKER:
            // Error register
            return StatusCode::Success;
            break;

        case RKCS:
            // Control Status register
            return StatusCode::Success;
            break;

        case RKWC:
            // Word Count register
            return StatusCode::Success;
            break;

        case RKBA:
            // Current Bus Address register
            return StatusCode::Success;
            break;

        case RKDA:
            // Disk Address register
            return StatusCode::Success;
            break;

        case RKDB:
            // Data Buffer register
            return StatusCode::Success;
            break;

        default:
            return (StatusCode::NonExistingMemory);
    }

    return StatusCode::Success;
}