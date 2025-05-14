#include "rk11d.h"

StatusCode RK11D::writeWord (BusAddress busAddress, u16 value)
{
    // Guard against controller register access from RK05 threads
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
            rkcs_.value = value & rkcsWritableBits;

            if (rkcs_.go)
            {
                // The Control Ready bit is to be cleared by GO being set
                rkcs_.controlReady = 0;

                // Disclaimer: the u16 resulting from the BitField conversion operator
                // cannot be cast directly to an Function enum.
                //
                // ToDo: Add Memory Extension bits to bus address
                //
                u16 function = rkcs_.operation;
                rk11ActionQueue_.push (RKTypes::Function
                    {
                        static_cast<RKTypes::Operation> (function),
                        rkda_.value, rkwc_, rkba_
                    });

                actionAvailable_.notify_one ();
            }

            // The controller is ready to accept a new command
            rkcs_.controlReady = 1;

            return StatusCode::Success;
            break;

        case RKWC:
            // Word Count register
            rkwc_ = value;
            return StatusCode::Success;
            break;

        case RKBA:
            // Current Bus Address register
            rkba_ = value;
            return StatusCode::Success;
            break;

        case RKDA:
            // Disk Address register
            rkda_.value = value;
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