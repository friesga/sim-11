#ifndef _ADDRESSINGMODE_TIMING_H_
#define _ADDRESSINGMODE_TIMING_H_

#include <cstdint>
#include <initializer_list>
#include <algorithm>
#include <array>
#include <cassert>

using u16 = uint16_t;

using std::initializer_list;
using std::array;
using std::copy_n;
using std::fill_n;

// This struct defines a table with an an addressing mode time per mode.
// The table contains eight rows, each row contains the timing for one
// addressing mode. The class containing the time per mode is templated
// to allow for differences in the timing per CPU type. The KDF11-NA timing
// e.g. varies for byte and word instructions, while the KDF11-A timing
// varies with the disabled/enabled state of the MMU.
//
template <typename T>
class AddressingModeTimes
{
public:
    AddressingModeTimes (initializer_list<T> times);
    AddressingModeTimes (T times);
    T const & operator[] (u16 index) const;

private:
    static const size_t numberOfAddressingModes {8};

    array<T, numberOfAddressingModes> times_;
};

// Definition of the constructors. The first constructor fills the table
// with a list of maximum eight AddressTime's, one for every mode. In case
// less AddressTimes's are specified the last value given is used for the
// remaining AddressTime's. This allows for the situation one AddressTime
// is given for addressing mode 0 and one for all other modes.
// The second constructor fills the table with the same time for all
// addressing modes.
template <typename T>
AddressingModeTimes<T>::AddressingModeTimes (initializer_list<T> times)
{
    size_t index {0};
    T const *elementPtr;

    assert (times.size () <= numberOfAddressingModes);

    for (elementPtr = times.begin (); elementPtr < times.end (); ++elementPtr)
        times_[index++] = *elementPtr;

    // Fill the uninitialized elements in the array with the last value
    // given in the arguments;
    --elementPtr;
    while (index < numberOfAddressingModes)
        times_[index++] = *elementPtr;
}

template <typename T>
AddressingModeTimes<T>::AddressingModeTimes (T times)
{
    times_.fill (times);
}

template <typename T>
T const & AddressingModeTimes<T>::operator[] (u16 index) const
{
    return times_[index];
}

#endif // _ADDRESSINGMODE_TIMIMG_H_