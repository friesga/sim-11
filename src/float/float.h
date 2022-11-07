#ifndef _FLOAT_H_
#define _FLOAT_H_

#include <cstdint>

// A class to handle PDP-11 floating point instructions. The constructor will
// accept a double word in PDP-11 FIS format and convert it to a floating
// point number in IEEE 754 format on which arithmetic operations can be
// performed. The result of the operations can be converted back to a PDP-11
// FIS double word.
// 
// The conversion routines are derived from the libvaxdata library by
// Lawrence M. Baker (https://pubs.usgs.gov/of/2005/1424/). For an explanation
// of the formats and the conversions see convert_vax_data.h in that library.
//
class Float
{
    friend Float operator+ (Float &left, Float const &right);
    friend Float operator- (Float &left, Float const &right);
    friend Float operator* (Float &left, Float const &right);
    friend Float operator/ (Float &left, Float const &right);

    union FloatUnion
    {
        float f32_;
        uint32_t u32_;
    };

    FloatUnion IEEEfloat1_;

    int exponent_;
    uint32_t mantissa_;

    // Private functions to perform conversion from PDP-11 to IEEE 754
    // format and vice versa.
    bool PDP11toIEEE (uint16_t const high, uint16_t const low, 
        uint32_t *ieeeOut);
    bool IEEEtoPDP11 (uint32_t const ieeeIn,
        uint16_t *high, uint16_t *low);

#if 0
    // Alternative implementation of PDP11toIEEE() producing more accurate
    // results for the VKACC1 subtraction diagnostics but worse results for
    // the additions.
    float single_precision_pdp11_to_ieee754 (uint16_t const high, uint16_t const low);
#endif

public:
    // Construct a Float from two words in PDP-11 FIS format
    Float (uint16_t high, uint16_t low);

    // Get the result as a IEEE 754 float
    float value ();
    operator float ();

    // Calculation operators
    Float operator+=(Float const &other);
    Float operator-=(Float const &other);
    Float operator*=(Float const &other);
    Float operator/=(Float const &other);

    // Get the result as two words in PDP-11 FIS format
    bool pdp11Dword(uint16_t *high, uint16_t *low);
};

#endif // _FLOAT_H_