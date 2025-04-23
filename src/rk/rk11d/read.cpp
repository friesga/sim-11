#include "rk11d.h"

CondData<u16> RK11D::read (BusAddress busAddress)
{
    // Decode registerAddress<3:1>
    switch (busAddress.registerAddress () & 016)
    {
        case RKDS:
            // Drive Status register
            return rkds_.value;
            break;

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
    }
}
