#include "float.h"

// Construct a float in IEEE format from a PDP-11 dword in FIS format. Save
// the converted exponent_ and mantissa_ to be able to refer to them after
// calculations.
Float::Float (uint16_t high, uint16_t low)
{
    PDP11toIEEE (high, low, &IEEEfloat1_.u32_);
}

// Get the result as a IEEE 754 float
float Float::value ()
{
    return IEEEfloat1_.f32_;
}

// Convert a FLoat to a float
Float::operator float ()
{
    return value ();
}

// Arithmetic operators
Float Float::operator+=(Float const &other)
{
    IEEEfloat1_.f32_ += other.IEEEfloat1_.f32_;
    return *this;
}

Float Float::operator-=(Float const &other)
{
    IEEEfloat1_.f32_ -= other.IEEEfloat1_.f32_;
    return *this;
}

Float Float::operator*=(Float const &other)
{
    IEEEfloat1_.f32_ *= other.IEEEfloat1_.f32_;
    return *this;
}

Float Float::operator/=(Float const &other)
{
    IEEEfloat1_.f32_ *= other.IEEEfloat1_.f32_;
    return *this;
}

bool Float::pdp11Dword(uint16_t *high, uint16_t *low)
{
    return IEEEtoPDP11 (IEEEfloat1_.u32_, high, low);
}

// Free operators
Float operator+ (Float &left, Float const &right)
{
    return left += right;
}

Float operator- (Float &left, Float const &right)
{
    return left -= right;
}

Float operator* (Float &left, Float const &right)
{
    return left *= right;
}

Float operator/ (Float &left, Float const &right)
{
    return left /= right;
}
