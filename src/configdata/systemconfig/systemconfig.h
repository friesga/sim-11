#ifndef _SYSTEMCONFIG_H_
#define _SYSTEMCONFIG_H_

#include "../deviceconfig/deviceconfig.h"

#include <vector>
#include <iterator>
#include <cstddef>
#include <ranges>
#include <initializer_list>

using std::vector;
using std::size_t;
using std::initializer_list;

// A system configuration comprises a number of device configurations. The
// class is equiped with an iterator to be able to iterate over the device
// configurations and has the functionality to be usable in ranges expressions.
//
class SystemConfig
{
public:
    class Iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = DeviceConfig;
        using pointer           = const DeviceConfig*;  // or also value_type*
        using reference         = const DeviceConfig&;  // or also value_type&

        // In the default constructor initialize the iterator with a default
        // initialized std::vector<DeviceConfig>::const_iterator.
        Iterator () : it_ () {}
        Iterator (std::vector<DeviceConfig>::const_iterator it) : it_ (it) {}
        reference operator* () const { return *it_; }
        pointer operator-> () { return &(*it_); }

        // Prefix increment
        Iterator& operator++ () { ++it_; return *this; }

        // Postfix increment
        Iterator operator++ (int) { Iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const Iterator& a, const Iterator& b) { return a.it_ == b.it_; }
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.it_ != b.it_; }

    private:
        std::vector<DeviceConfig>::const_iterator it_;
    };

    // Allow the system configuration to be initialized with a list of device
    // configurations
    SystemConfig (initializer_list<DeviceConfig> devices) : devices_ (devices) {}

    void addDeviceConfig (const DeviceConfig& device);

    Iterator begin () const { return Iterator (devices_.begin ()); }
    Iterator end () const { return Iterator (devices_.end ()); }

    DeviceConfig const& operator[] (size_t index) const { return devices_[index]; }

    // Call operator voor ranges
    auto operator()() const
    {
        return std::ranges::subrange<Iterator, Iterator> (begin (), end ());
    }

private:
    vector<DeviceConfig> devices_ {};
};

inline void SystemConfig::addDeviceConfig (const DeviceConfig& device)
{
    devices_.push_back (device);
}

#endif // _SYSTEMCONFIG_H_
