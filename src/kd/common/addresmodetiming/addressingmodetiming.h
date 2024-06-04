#ifndef _ADDRESSINGMODE_TIMING_H_
#define _ADDRESSINGMODE_TIMING_H_

#include "types.h"

#include <initializer_list>

using std::initializer_list;
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
    AddressingModeTimes (T mode0Time, T mode1_7Time);
    T const & operator[] (u16 index) const;

private:
    array<T, 8> times_;
};

// Definition of the constructors. The first constructor fills the table
// with a list of eight AddressTime's, one for every mode. The second
// constructor fills the table with the same time for all addressing modes
// and the last constructor takes two times, one for addressing mode 0
// and one for the other addressing modes.
template <typename T>
AddressingModeTimes<T>::AddressingModeTimes (initializer_list<T> times)
{
    copy_n (times.begin (), 8, times_.begin ());
}

template <typename T>
AddressingModeTimes<T>::AddressingModeTimes (T times)
{
    times_.fill (times);
}

template <typename T>
AddressingModeTimes<T>::AddressingModeTimes (T mode0Time, T mode1_7Time)
{
    times_[0] = mode0Time;
    fill_n (times_ + 1, 7, mode1_7Time);
}

template <typename T>
T const & AddressingModeTimes<T>::operator[] (u16 index) const
{
    return times_[index];
}

#endif // _ADDRESSINGMODE_TIMIMG_H_