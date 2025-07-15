#include "geometry.h"

// Calculate the disk address from the given logical block number
// using the defined geometry
//
DiskAddress Geometry::lbnTodiskAddress (u32 lbn) const
{
    DiskAddress da;
    da.cylinder = lbn / (sectorsPerSurface_ * numberOfHeads_);
    da.head = (lbn / sectorsPerSurface_) % numberOfHeads_;
    da.sector = lbn % sectorsPerSurface_;
    return da;
}
