#ifndef _MEMORYOPERANDLOCATION_H_
#define _MEMORYOPERANDLOCATION_H_

#include "kd/include/cpudata.h"
#include "kd/include/mmu.h"
#include "conddata/conddata.h"

class MemoryOperandLocation
{
public:
    MemoryOperandLocation (CpuData* cpuData, MMU* mmu, CondData<u16> memoryAddress);
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
    CondData <u16> location_;
    CpuData* cpuData_;
    MMU* mmu_;
};

template <typename T>
requires std::same_as<T, CondData<u16>>
T MemoryOperandLocation::contents ()
{
    return mmu_->fetchWord (location_);
}

template <typename T>
requires std::same_as<T, CondData<u8>>
T MemoryOperandLocation::contents ()
{
    return mmu_->fetchByte (location_);
}

// Return the contents of the operand location in the previous memory
// management mode. 
template <typename T>
requires std::same_as<T, CondData<u16>>
T MemoryOperandLocation::prevModeContents ()
{
    return mmu_->fetchWord (location_, PSW::Mode::Previous);
}

template <typename T>
requires std::same_as<T, CondData<u8>>
T MemoryOperandLocation::prevModeContents ()
{
    return mmu_->fetchByte (location_, PSW::Mode::Previous);
}

template <typename T>
requires std::same_as<T, u16>
bool MemoryOperandLocation::write (T contents)
{
    return mmu_->putWord (location_, contents);
}

template <typename T>
requires std::same_as<T, u8>
bool MemoryOperandLocation::write (T contents)
{
    return mmu_->putByte (location_, contents);
}

// Write the contents to the memory location using the previous memory
// management mode
template <typename T>
requires std::same_as<T, u16>
bool MemoryOperandLocation::writePrevMode (T contents)
{
    return mmu_->putWord (location_, contents, PSW::Mode::Previous);
}

template <typename T>
requires std::same_as<T, u8>
bool MemoryOperandLocation::writePrevMode (T contents)
{
    return mmu_->putByte (location_, contents, PSW::Mode::Previous);
}
#endif // _MEMORYOPERANDLOCATION_H_