#ifndef _MEMORYOPERANDLOCATION_H_
#define _MEMORYOPERANDLOCATION_H_

#include "../../cpudata.h"
#include "conddata/conddata.h"

class MemoryOperandLocation
{
public:
    MemoryOperandLocation (CpuData *cpu, CondData<u16> memoryAddress);
    bool isValid ();
    operator u16 ();
    template <typename T> requires std::same_as<T, CondData<u16>> T contents ();
    template <typename T> requires std::same_as<T, CondData<u8>> T contents ();
    template <typename T> requires std::same_as<T, u16> bool write (T contents);
    template <typename T> requires std::same_as<T, u8> bool write (T contents);

private:
    CondData <u16> location_;
    CpuData *cpu_;
};

template <typename T>
requires std::same_as<T, CondData<u16>>
T MemoryOperandLocation::contents ()
{
    return cpu_->fetchWord (location_);
}

template <typename T>
requires std::same_as<T, CondData<u8>>
T MemoryOperandLocation::contents ()
{
    return cpu_->fetchByte (location_);
}

template <typename T>
requires std::same_as<T, u16>
bool MemoryOperandLocation::write (T contents)
{
    return cpu_->putWord (location_, contents);
}


template <typename T>
requires std::same_as<T, u8>
bool MemoryOperandLocation::write (T contents)
{
    return cpu_->putByte (location_, contents);
}

#endif // _MEMORYOPERANDLOCATION_H_