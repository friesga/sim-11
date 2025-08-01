#include "rk11d.h"


// The Read Check function is identical to a normal Read function, except
// that no NPRs occur. Only the checksum is calculated and compared with the
// checksum read from the disk drive. This function enables the program to
// know beforehand if a given block of data is readable and error free.
// Because the Read Check is essentially a parity check, it must be performed
// on a whole-sector basis only. (EK-RK11D-MM-002)
// 
// As the parity check is not implemented this function essentially is a NOP
// and just checks the parameters.
//
void RK11D::executeReadCheck (RKTypes::Function function)
{
    u16 driveId = function.diskAddress.driveSelect;

    if (!driveReady (function))
        return;

    // Check validity of the function's parameters
    if (!functionParametersOk (function))
        return;
}