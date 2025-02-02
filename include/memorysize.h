#ifndef _MEMORYSIZE_H_
#define _MEMORYSIZE_H_

#include <cstddef>

// The unit for memory size is a byte
struct Bytes
{
    constexpr Bytes (size_t value) : value_ (value) {}
    const size_t value_;
};

// Definition of the memory capacity in number of bytes
class memorySize
{
public:
    // Define the memory capacity via a Bytes object. This prevents the
    // definition of capacity by means of an integer thus preventing confusion
    // about the used unit.
    constexpr memorySize (Bytes bytes)
        :
        numBytes_ {bytes}
    {}

    // Function to return capacity as number of bytes
    constexpr size_t byteCapacity () const noexcept
    {
        return numBytes_.value_;
    }

    // Function to return capacity as number of words
    constexpr size_t wordCapacity () const noexcept
    {
        return numBytes_.value_ / 2;
    }

private:
    const Bytes numBytes_;
};

constexpr memorySize operator"" _KiB (unsigned long long num)
{
    return Bytes {num * 1024};
}

constexpr memorySize operator"" _KiW (unsigned long long num)
{
    return Bytes {num * 1024 * 2};
}

#endif // _MEMORYSIZE_H_