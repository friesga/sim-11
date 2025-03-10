#ifndef _EMPTYOPERANDLOCATION_H_
#define _EMPTYOPERANDLOCATION_H_

#include "types.h"
#include "conddata/conddata.h"

#include <stdexcept>

using std::invalid_argument;

class EmptyOperandLocation
{
public:
    EmptyOperandLocation ();
    bool isValid ();
    operator u16 ();
    template <typename T> requires std::same_as<T, CondData<u16>> T contents ();
    template <typename T> requires std::same_as<T, CondData<u8>> T contents ();
    template <typename T> requires std::same_as<T, CondData<u16>> T prevModeContents ();
    template <typename T> requires std::same_as<T, CondData<u8>> T prevModeContents ();
    template <typename T> requires std::same_as<T, u16> bool write (T contents);
    template <typename T> requires std::same_as<T, u8> bool write (T contents);
    template <typename T> requires std::same_as<T, u16> bool writePrevMode (T contents);
    template <typename T> requires std::same_as<T, u8> bool writePrevMode (T contents);

};

template <typename T>
requires std::same_as<T, CondData<u16>>
T EmptyOperandLocation::contents ()
{
    throw invalid_argument ("Read access on empty operand location");
}

template <typename T>
requires std::same_as<T, CondData<u8>>
T EmptyOperandLocation::contents ()
{
    throw invalid_argument ("Read access on empty operand location");
}

template <typename T>
requires std::same_as<T, CondData<u16>>
T EmptyOperandLocation::prevModeContents ()
{
    throw invalid_argument ("Read access on empty operand location");
}

template <typename T>
requires std::same_as<T, CondData<u8>>
T EmptyOperandLocation::prevModeContents ()
{
    throw invalid_argument ("Read access on empty operand location");
}

template <typename T>
requires std::same_as<T, u16>
bool EmptyOperandLocation::write (T contents)
{
    throw invalid_argument ("Write access on empty operand location");
}

template <typename T>
requires std::same_as<T, u8>
bool EmptyOperandLocation::write (T contents)
{
    throw invalid_argument ("Write access on empty operand location");
}

template <typename T>
requires std::same_as<T, u16>
bool EmptyOperandLocation::writePrevMode (T contents)
{
    throw invalid_argument ("Write access on empty operand location");
}

template <typename T>
requires std::same_as<T, u8>
bool EmptyOperandLocation::writePrevMode (T contents)
{
    throw invalid_argument ("Write access on empty operand location");
}

#endif // !_EMPTYOPERANDLOCATION_H_




