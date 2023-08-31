#ifndef _EMPTYOPERANDLOCATION_H_
#define _EMPTYOPERANDLOCATION_H_

#include "types.h"
#include "conddata/conddata.h"

#include <string>

using std::string;

class EmptyOperandLocation
{
public:
    EmptyOperandLocation ();
    bool isValid ();
    operator u16 ();
    template <typename T> requires std::same_as<T, u16> CondData<T> contents ();
    template <typename T> requires std::same_as<T, u8> CondData<T> contents ();
    template <typename T> requires std::same_as<T, u16> bool write (T contents);
    template <typename T> requires std::same_as<T, u8> bool write (T contents);

};

template <typename T>
requires std::same_as<T, u16>
CondData<T> EmptyOperandLocation::contents ()
{
    throw string ("Read access on empty operand location");
}

template <typename T>
requires std::same_as<T, u8>
CondData<T> EmptyOperandLocation::contents ()
{
    throw string ("Read access on empty operand location");
}

template <typename T>
requires std::same_as<T, u16>
bool EmptyOperandLocation::write (T contents)
{
    throw string ("Write access on empty operand location");
}

template <typename T>
requires std::same_as<T, u8>
bool EmptyOperandLocation::write (T contents)
{
    throw string ("Write access on empty operand location");
}

#endif // !_EMPTYOPERANDLOCATION_H_




