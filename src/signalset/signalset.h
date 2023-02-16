#ifndef _SIGNALSET_H_
#define _SIGNALSET_H_

#include <array>

using std::array;

class SignalSet
{
public:
    // Define a plain enum to avoid an additional scope level and so its
    // numeric value can be used as array index without casting.
    enum Signal
    {
        BDCOK,
        BHALT,
        Count
    };

    void set (Signal signal, bool value);
    bool isSet (Signal signal);

private:
    array<bool, Signal::Count> signals_ {false};
};


#endif // _SIGNALSET_H_