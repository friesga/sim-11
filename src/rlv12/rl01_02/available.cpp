#include "rl01_02.h"

// A unit is available if a file is attached.
//
// In principle several conditions exist in which a drive is not available:
// - No drive connected,
// - No unit number present in the bezel,
// - No file attached.
// 
// In all of those cases the drive there is no file attached and the
// drive therefore is not available.
//
bool RL01_02::available ()
{
    if (unitStatus_ & Status::UNIT_ATT)
        return true;

    return false;
}