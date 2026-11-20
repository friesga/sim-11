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
            return getDriveStatus (selectedDrive_).value;

        case Offset::RKER:
            // Error register
            return rker_.value;

        case Offset::RKCS:
            // Control Status register
            return rkcs_.value;

        case Offset::RKWC:
            // Word Count register
            return rkwc_;
            
        case Offset::RKBA:
            // Current Bus Address register
            return rkba_;

        case Offset::RKDA:
            // Disk Address register
            return rkda_.value;

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
