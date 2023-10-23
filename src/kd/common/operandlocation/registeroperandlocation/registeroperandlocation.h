#ifndef _REGISTEROPERANDLOCATION_H_
#define _REGISTEROPERANDLOCATION_H_

#include "kd/include/cpudata.h"
#include "conddata/conddata.h"

#include <type_traits>

class RegisterOperandLocation
{
public:
    RegisterOperandLocation (CpuData *cpu, u8 registerNumber);
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

private:
    u8 location_;
    CpuData *cpu_;
};

template <typename T>
requires std::same_as<T, CondData<u16>>
T RegisterOperandLocation::contents ()
{
    return CondData<u16> {cpu_->registers () [location_]};
}

template <typename T>
requires std::same_as<T, CondData<u8>>
T RegisterOperandLocation::contents ()
{
    return CondData<u8> {static_cast<u8> 
        (cpu_->registers () [location_] & 0377)};
}

// Return the contents of the operand location in the previous memory
// management mode
template <typename T>
requires std::same_as<T, CondData<u16>>
T RegisterOperandLocation::prevModeContents ()
{
    return CondData<u16> {cpu_->registers ().prevModeContents (location_)};
}

template <typename T>
requires std::same_as<T, CondData<u8>>
T RegisterOperandLocation::prevModeContents ()
{
    return CondData<u8> {static_cast<u8> 
        (cpu_->registers ().prevModeContents (location_) & 0377)};
}

template <typename T> 
requires std::same_as<T, u16> 
bool RegisterOperandLocation::write (T contents)
{
    cpu_->registers () [location_] = contents;
    return true;
}

template <typename T>
requires std::same_as<T, u8>
bool RegisterOperandLocation::write (T contents)
{
    u16 regNr = location_;
    cpu_->registers () [regNr] = (cpu_->registers () [regNr] & 0xFF00) | contents;
    return true;
}

template <typename T> 
requires std::same_as<T, u16> 
bool RegisterOperandLocation::writePrevMode (T contents)
{
    cpu_->registers ().writePrevMode (location_, contents);
    return true;
}

template <typename T>
requires std::same_as<T, u8>
bool RegisterOperandLocation::writePrevMode (T contents)
{
    u16 originalContents = cpu_->registers ().prevModeContents (location_);
    cpu_->registers ().writePrevMode (location_, 
        (originalContents & 0xFF00) | contents);
    return true;
}
#endif // !_REGISTEROPERANDLOCATION_H_




