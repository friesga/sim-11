#ifndef _KD11NAADDRESSINGMODETIME_H_
#define _KD11NAADDRESSINGMODETIME_H_

// Definition of the timimg for a specific adressing mode. On the KD11-NA
// the timing varies between byte and word instructions.
struct KD11_NA_AddressingModeTime
{
    KD11_NA_AddressingModeTime ();
    KD11_NA_AddressingModeTime (double byte, double word);
    KD11_NA_AddressingModeTime (double bytePlusWord);
    double byte_;
    double word_;
};


inline KD11_NA_AddressingModeTime::KD11_NA_AddressingModeTime ()
    :
    byte_ {0.0},
    word_ {0.0}
{}

inline KD11_NA_AddressingModeTime::KD11_NA_AddressingModeTime (double byte, double word)
    :
    byte_ {byte},
    word_ {word}
{}

inline KD11_NA_AddressingModeTime::KD11_NA_AddressingModeTime (double bytePlusWord)
    :
    byte_ {bytePlusWord},
    word_ {bytePlusWord}
{}

#endif // _KD11NAADDRESSINGMODETIME_H_