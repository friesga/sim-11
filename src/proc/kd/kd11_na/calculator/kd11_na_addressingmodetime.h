#ifndef _KD11NAADDRESSINGMODETIME_H_
#define _KD11NAADDRESSINGMODETIME_H_

// Definition of the timimg for a specific adressing mode. On the KD11-NA
// the timing varies between byte and word instructions.
struct KD11_NA_AddressingModeTime
{
    KD11_NA_AddressingModeTime ();
    KD11_NA_AddressingModeTime (double word, double byte);
    KD11_NA_AddressingModeTime (double wordPlusByte);
    double word_;
    double byte_;
};


inline KD11_NA_AddressingModeTime::KD11_NA_AddressingModeTime ()
    :
    word_ {0.0},
    byte_ {0.0}
{}

inline KD11_NA_AddressingModeTime::KD11_NA_AddressingModeTime (double word, double byte)
    :
    word_ {word},
    byte_ {byte}
{}

inline KD11_NA_AddressingModeTime::KD11_NA_AddressingModeTime (double wordPlusByte)
    :
    word_ {wordPlusByte},
    byte_ {wordPlusByte}
{}

#endif // _KD11NAADDRESSINGMODETIME_H_