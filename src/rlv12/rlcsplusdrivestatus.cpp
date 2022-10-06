#include "rlv12.h"

//
// This function determines the Drive Ready and Drive Status bits from
// the given unit and combines these bits with the current CSR to a 
// value to be returned to the caller. In case Drive Error is set,
// Composite Error is also set.
// 
// The DRDY signal is sent by the selected drive to indicate that
// it is ready to read or write or seek.  It is sent when the
// heads are not moving and are locked onto a cylinder.  This is
// continuously monitored by the drive and controller.
// (EK-0RL11-TD-001, p. 3-8)
//
// DR RDY will be negated [i.e. cleared] when a drive error
// occurs except when an attempt has been made to write on a
// write-protected drive or if volume check is set. In either
// case, only DR ERR will be asserted. (EK-RLV12-TD-001, p3-7)
// 
// This means that on a write to a write-protected drive or if
// volume check is set, DR ERR is set and DR RDY is cleared.
//
// The Drive Ready (and Drive Error) bits are taken over from the
// currently selected drive.
u16 RLV12::rlcsPlusDriveStatus (RL01_2 &unit)
{
    u16 rlcsCombined {rlcs};

    if (unit.driveStatus_ & RLDS_ERR)
        rlcsCombined |= RLCS_DRE;
    else
        rlcsCombined &= ~RLCS_DRE;

    // The drive is ready if it is not disconnected, no error is
    // reported by the drive and the heads are locked on a cylinder.
    if (!(unit.unitStatus_ & Status::UNIT_DIS) && 
        !(unit.driveStatus_ & RLDS_DRRDY_NEGATE) &&
        (unit.driveStatus_ & RLDS_M_STATE) == RLDS_LOCK)
        rlcsCombined |= RLCS_DRDY;
    else
        rlcsCombined &= ~RLCS_DRDY;

    // Make sure the error summary bit properly reflects the 
    // sum of other errors.
    if (rlcsCombined & RLCS_ALLERR)
        rlcsCombined |= RLCS_ERR;
    else
        rlcsCombined &= ~RLCS_ERR;

    return rlcsCombined;
}