#ifndef _ADDRESSTIMES_H_
#define _ADDRESSTIMES_H_

#include <cstdint>
#include <array>
#include <initializer_list>
#include <algorithm>

using std::array;
using std::initializer_list;
using std::copy_n;
using std::copy;

// This struct describes one row of an AddressTimes table.
struct AddressTime
{
    double MMEnabled;
    double MMDisabled;
};

// This struct defines an Address Times table as defined in Appendix B
// of KDF11-B User's Guide. Each table comprises a row per mode (0..7).
class AddressTimes
{
public:
    AddressTimes (initializer_list<AddressTime> times);
    AddressTimes (AddressTime times);
    double getMMUEnabledTime (size_t mode) const;
    double getMMUDisabledTime (size_t mode) const;

private:
    array<AddressTime, 8> times_;
};


inline AddressTimes::AddressTimes (initializer_list<AddressTime> times)
{
    copy_n (times.begin (), 8, times_.begin ());
}


inline AddressTimes::AddressTimes (AddressTime times)
{
    times_.fill (times);
}

inline double AddressTimes::getMMUEnabledTime (size_t mode) const
{
    return times_[mode].MMEnabled;
}

inline double AddressTimes::getMMUDisabledTime (size_t mode) const
{
    return times_[mode].MMDisabled;
}

#endif // _ADDRESSTIMES_H_