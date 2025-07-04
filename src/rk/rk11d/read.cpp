#include "rk11d.h"
#include "trace/trace.h"

CondData<u16> RK11D::read (BusAddress busAddress)
{
    trace.rk11Registers (busAddress, rker_, getDriveStatus (selectedDrive_),
        rkcs_, rkwc_, rkba_, rkda_, rkdb_);

    // Decode registerAddress<3:1>
    switch (busAddress.registerAddress () & 016)
    {
        case RKDS:
            // Drive Status register
            return getDriveStatus (selectedDrive_).value;

        case RKER:
            // Error register
            return rker_.value;

        case RKCS:
            // Control Status register
            return rkcs_.value;

        case RKWC:
            // Word Count register
            return rkwc_;
            
        case RKBA:
            // Current Bus Address register
            return rkba_;

        case RKDA:
            // Disk Address register
            return rkda_.value;

        case RKMR:
            // Maintenance register
            return 0;

        case RKDB:
            // Data Buffer register
            return rkdb_;

        default:
            return (StatusCode::NonExistingMemory);
    }
}
