#ifndef _MEMORYOPERANDLOCATION_H_
#define _MEMORYOPERANDLOCATION_H_

#include "proc/include/cpudata.h"
#include "proc/common/datapaths/datapaths.h"
#include "conddata/conddata.h"

class MemoryOperandLocation
{
public:
    MemoryOperandLocation (CpuData* cpuData, DataPaths* dataPaths,
        CondData<u16> memoryAddress);
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
    DataPaths* dataPaths_;
};

template <typename T>
requires std::same_as<T, CondData<u16>>
T MemoryOperandLocation::contents ()
{
    return dataPaths_->fetchWord (static_cast<BusAddress> (location_));
}

template <typename T>
requires std::same_as<T, CondData<u8>>
T MemoryOperandLocation::contents ()
{
    return dataPaths_->fetchByte (static_cast<BusAddress> (location_));
}

// Return the contents of the operand location in the previous memory
// management mode. 
template <typename T>
requires std::same_as<T, CondData<u16>>
T MemoryOperandLocation::prevModeContents ()
{
    return dataPaths_->fetchWord (static_cast<BusAddress> (location_),
        PSW::Mode::Previous);
}

template <typename T>
requires std::same_as<T, CondData<u8>>
T MemoryOperandLocation::prevModeContents ()
{
    return dataPaths_->fetchByte (static_cast<BusAddress> (location_),
        PSW::Mode::Previous);
}

template <typename T>
requires std::same_as<T, u16>
bool MemoryOperandLocation::write (T contents)
{
    return dataPaths_->putWord (static_cast<BusAddress> (location_), contents);
}

template <typename T>
requires std::same_as<T, u8>
bool MemoryOperandLocation::write (T contents)
{
    return dataPaths_->putByte (static_cast<BusAddress> (location_), contents);
}

// Write the contents to the memory location using the previous memory
// management mode
template <typename T>
requires std::same_as<T, u16>
bool MemoryOperandLocation::writePrevMode (T contents)
{
    return dataPaths_->putWord (static_cast<BusAddress> (location_), contents,
        PSW::Mode::Previous);
}

template <typename T>
requires std::same_as<T, u8>
bool MemoryOperandLocation::writePrevMode (T contents)
{
    return dataPaths_->putByte (static_cast<BusAddress> (location_), contents,
        PSW::Mode::Previous);
}
#endif // _MEMORYOPERANDLOCATION_H_