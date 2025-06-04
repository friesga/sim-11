#include "rk11d.h"

// For a Seek function, the RK11 directs the selected disk drive to move its
// head mechanism to the cylinder address specified by RKDA 05 through 12.
// When this portion of a Seek has been initiated, the controller returns to
// the Ready state (RKCS 07). But if the specified cylinder address is greater
// than 0312, the function is aborted and bit 06 (nonexistent Cylinder) of the
// RKER is set. RKCS 06 (Interrupt Done Enable) then determines the program
// reaction. (EK-RK11D-MM-002 par. 1.3.2.2)
//
// The acceptance (Address Acknowledge) of a Seek or Srive Reset function by
// the selected drive generates an interrupt request.
// (EK-RK11D-MM-002, par. 3.4)
// 
// Presumably the interrupt is only generated when RKCS IDE is set.
//
void RK11D::executeSeek (RKTypes::RKDA diskAddress)
{
    if (diskAddress.cylinderAddress < RKTypes::CylindersPerDisk)
        rk05Drives_[diskAddress.driveSelect]->seek (diskAddress.cylinderAddress);
    else
    {
        rker_.nonexistentCylinder = 1;
        rkcs_.error = 1;
        rkcs_.hardError = 1;
    }

    setControlReady ();
}