#ifndef _KDF11ADDRESSINGMODETIME_H_
#define _KDF11ADDRESSINGMODETIME_H_

// Definition of the timimg for a specific adressing mode. On the KD11-NA
// the timing varies between byte and word instructions.
struct KDF11_AddressingModeTime
{
    KDF11_AddressingModeTime ();
    KDF11_AddressingModeTime (double MMEnabled, double MMDisabled);
    KDF11_AddressingModeTime (double EnabledPlusDisabled);
    double MMEnabled_;
    double MMDisabled_;
};


inline KDF11_AddressingModeTime::KDF11_AddressingModeTime ()
    :
    MMEnabled_ {0.0},
    MMDisabled_ {0.0}
{}

inline KDF11_AddressingModeTime::KDF11_AddressingModeTime (double MMEnabled, double MMDisabled)
    :
    MMEnabled_ {MMEnabled},
    MMDisabled_ {MMDisabled}
{}

inline KDF11_AddressingModeTime::KDF11_AddressingModeTime (double EnabledPlusDisabled)
    :
    MMEnabled_ {EnabledPlusDisabled},
    MMDisabled_ {EnabledPlusDisabled}
{}

#endif // _KDF11ADDRESSINGMODETIME_H_