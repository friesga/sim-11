#include "rk11d.h"
#include "trace/trace.h"

CondData<u16> RK11D::read (BusAddress busAddress)
{
    trace.rk11Registers (busAddress, rker_, getDriveStatus (selectedDrive_),
        rkcs_, rkwc_, rkba_, rkda_, rkdb_);

    // Decode registerAddress<3:1>
    switch (busAddress.registerAddress () & 016)
    {
        case Offset::RKDS:
            // Drive Status register
            return static_cast<u16> (getDriveStatus (selectedDrive_));

        case Offset::RKER:
            // Error register
            return static_cast<u16> (rker_);

        case Offset::RKCS:
            // Control Status register
            return static_cast<u16> (rkcs_);

        case Offset::RKWC:
            // Word Count register
            return rkwc_;
            
        case Offset::RKBA:
            // Current Bus Address register
            return rkba_;

        case Offset::RKDA:
            // Disk Address register
            return static_cast<u16> (rkda_);

        case Offset::RKMR:
            // Maintenance register
            return 0;

        case Offset::RKDB:
            // Data Buffer register
            return rkdb_;

        default:
            return (StatusCode::NonExistingMemory);
    }
}
