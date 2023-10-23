#ifndef _OPERANDLOCATION_H_
#define _OPERANDLOCATION_H_

#include "emptyoperandlocation/emptyoperandlocation.h"
#include "registeroperandlocation/registeroperandlocation.h"
#include "memoryoperandlocation/memoryoperandlocation.h"

#include "types.h"
#include "kd/include/cpudata.h"
#include "conddata/conddata.h"

#include <variant>
#include <type_traits>

using std::variant;


// A location of an instruction operand is either a register or a memory
// location in the given CPU. A memory location is in the form of a CondData
// object. This has the advantage that std::variant can discriminate between
// these two types.
//
// The cpu parameter is used to be able to retrieve the contents of the
// location.
class OperandLocation
{
public:
    OperandLocation ();
    ~OperandLocation () {};
    template <typename T>
    OperandLocation (const T& operandLocation);
    template <typename T> void operator= (const T& operandLocation);
    template <typename T> bool isA ();
    bool isValid ();
    operator u16 ();
    template <typename T> CondData<T> contents ();
    template <typename T> CondData<T> prevModeContents ();
    template <typename T> bool write (T contents);

private:
    variant <EmptyOperandLocation, RegisterOperandLocation, MemoryOperandLocation> location_;
};

template <typename T>
OperandLocation::OperandLocation (const T& operandLocation)
    :
    location_ {operandLocation}
{}

template <typename T>
inline bool OperandLocation::isA ()
{
    return holds_alternative<T> (location_);
}

// Assignment operator
template <typename T>
void OperandLocation::operator= (const T& operandLocation)
{
    location_ = operandLocation;
}

// Return the contents of the location pointed to by this OperandLocation. The
// location is either a register number or a memory address. In the first case
// the contents of the register are returned, in the second case the contents
// of the memory address.
//
// Note the syntax in the contents<T> call below. MSVCC accept the syntax
// "obj.contents<T> ()" but gcc has to be informed about the nature of the
// call. See C++ Annotations par. 23.1.2.
template <typename T>
CondData<T> OperandLocation::contents ()
{
    return std::visit ([] (auto &obj) -> CondData<T> 
         {return obj.template contents<T> (); }, location_);
}

// Return the contents of the operand location in the previous memory
// management mode.
template <typename T>
CondData<T> OperandLocation::prevModeContents ()
{
    return std::visit ([] (auto &obj) -> CondData<T> 
         {return obj.template prevModeContents<T> (); }, location_);
}

template <typename T>
bool OperandLocation::write (T contents)
{
    return std::visit ([contents] (auto &obj) -> bool 
        {return obj.template write<T> (contents); }, location_);
}

#endif // _OPERANDLOCATION_H_