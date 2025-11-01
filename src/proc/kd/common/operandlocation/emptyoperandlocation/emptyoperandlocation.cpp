#include "emptyoperandlocation.h"

#include <string>

using std::string;
using std::invalid_argument;

EmptyOperandLocation::EmptyOperandLocation ()
{}

// Return whether or not the OperandLocation points to a valid address
bool EmptyOperandLocation::isValid ()
{
    return false;
}

EmptyOperandLocation::operator u16 ()
{
    throw invalid_argument ("Read access on empty operand location");    
}

