#include "emptyoperandlocation.h"

#include <string>

using std::string;


EmptyOperandLocation::EmptyOperandLocation ()
{}

// Return whether or not the OperandLocation points to a valid address
bool EmptyOperandLocation::isValid ()
{
    return false;
}

EmptyOperandLocation::operator u16 ()
{
    throw string ("Read access on empty operand location");    
}

