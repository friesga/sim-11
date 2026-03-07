#include "geometry.h"

#include <stdexcept>

using std::out_of_range;

// Calculate the disk address from the given logical block number
// using the defined geometry
//
DiskAddress Geometry::lbnTodiskAddress (u32 lbn) const
{
    if (!validLBN (lbn))
        throw out_of_range ("invalid lbn");

    DiskAddress da;
    da.cylinder = lbn / (sectorsPerSurface_ * numberOfHeads_);
    da.head = (lbn / sectorsPerSurface_) % numberOfHeads_;
    da.sector = lbn % sectorsPerSurface_;
    return da;
}

bool Geometry::validLBN (u32 lbn) const
{
    return lbn < diskCapacity_;
}
