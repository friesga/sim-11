#include "operandlocation.h"

using std::get;
using std::holds_alternative;

// Allow an uninitialized OperandLocation to be able to define it as a
// class memeber.
OperandLocation::OperandLocation ()
    :
    location_ {EmptyOperandLocation {}}
{}

// Return whether or not the OperandLocation points to a valid address
bool OperandLocation::isValid ()
{
    return std::visit ([] (auto &obj) -> bool 
        {return obj.isValid (); }, location_);
}

// Return, depending on (the type of) the value held in the location, either
// the register number or the memory address held in the location. The type
// of the value can be determined by means of the isA<> function.
OperandLocation::operator u16 ()
{
    return std::visit ([] (auto &obj) -> u16 
         {return obj; }, location_);
}
