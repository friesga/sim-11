#include "rk11d.h"

StatusCode RK11D::writeWord (BusAddress busAddress, u16 value)
{
    // Guard against controller register access from RK05 threads
    std::lock_guard<std::mutex> guard {controllerMutex_};

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
                // (EK-RK11D-MM-002 p. 3-6)
                rkcs_.controlReady = 0;

                // Disclaimer: the u16 resulting from the BitField conversion operator
                // cannot be cast directly to an Function enum.
                //
                // ToDo: Add Memory Extension bits to bus address
                //
                u16 operation = rkcs_.operation;
                actionQueue_.push (RKTypes::Function
                    {
                        static_cast<RKTypes::Operation> (operation),
                        rkda_.value, rkwc_, rkba_
                    });

                actionAvailable_.notify_one ();
            }

            // The controller is ready to accept a new command
            // According to EK-RK11D-MM-002 p. 3-4, the GO bit remains set
            // until the control actually begins to respond to GO, which may
            // take from 1 microsecond to 3.3 milliseconds, depending on the
            // current operation of the selected drive.
            rkcs_.go = 0;

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