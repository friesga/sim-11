#include "rk11d.h"

CondData<u16> RK11D::read (BusAddress busAddress)
{
    // Decode registerAddress<3:1>
    switch (busAddress.registerAddress () & 016)
    {
        case RKDS:
            // Drive Status register
            if (selectedDrive_ > rk05Drives_.size ())
                return rk05Drives_[selectedDrive_]->driveStatus ();
            else
            {
                RKTypes::RKDS rkds {};
                rkds.driveId = selectedDrive_;
                rkds.drivePowerLow = 1;
                rkds.driveUnsafe = 1;
                rkds.driveReady = 0;
                return rkds.value;
            }

        case RKER:
            // Error register
            return rker_.value;
            break;

        case RKCS:
            // Control Status register
            return rkcs_.value;
            break;

        case RKWC:
            // Word Count register
            return rkwc_;
            break;
            
        case RKBA:
            // Current Bus Address register
            return rkba_;
            break;

        case RKDA:
            // Disk Address register
            return rkda_.value;
            break;

        case RKDB:
            // Data Buffer register
            return rkdb_;
            break;

        default:
            return (StatusCode::NonExistingMemory);
    }
}
