#ifndef _READONLYREGISTER_H_
#define _READONLYREGISTER_H_

#include "types.h"
#include "register.h"

// Definition of read-only register. Writes to the register are ignored.
// The class forms a kind of facade or decorator which removes the possibility
// to write to a register from the specified register.
//
class ReadOnlyRegister : public Register
{
public:
    ReadOnlyRegister (Register const &reg);
    void operator= (u16 const value) override;
    operator u16 () const override;

private:
    Register const &register_;
};

inline ReadOnlyRegister::ReadOnlyRegister (Register const &reg)
    : 
    register_ {reg}
{}

// Ignore the write
inline void ReadOnlyRegister::operator= (u16 const value)
{}

inline ReadOnlyRegister::operator u16 () const
{
    return register_;
}

#endif // _READONLYREGISTER_H_