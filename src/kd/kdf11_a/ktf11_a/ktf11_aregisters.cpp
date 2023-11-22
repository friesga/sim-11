#ifndef _KTF11AREGISTERS_H_
#define _KTF11AREGISTERS_H_

#include "ktf11_a.h"
#include "kd/include/psw.h"

u16* KTF11_A::registerPointer (u16 address)
{
    switch (address)
    {
        case statusRegister0:
            return &sr0_;

        case statusRegister1:
            return &sr1_;

        case statusRegister2:
            return &sr2_;

        case statusRegister3:
            return &sr3_;

        default:
            // The twelve highest bits indicate the register set while the
            // lower four bits indicate the the register number within
            // the set.
            u16 index = (address & 017) / 2;
            switch (address & 0177760)
            {
                case kernelPARBase:
                    return &activePageRegisterSet_[KERNEL_MODE]
                        .activePageRegister_[index]
                        .PageAddressRegister_;

                case kernelPDRBase:
                    return &activePageRegisterSet_[KERNEL_MODE]
                        .activePageRegister_[index]
                        .PageDescripterRegister_;

                case userPARBase:
                    return &activePageRegisterSet_[USER_MODE]
                        .activePageRegister_[index]
                        .PageAddressRegister_;

                case userPDRBase:
                    return &activePageRegisterSet_[USER_MODE]
                        .activePageRegister_[index]
                        .PageDescripterRegister_;

                default:
                    return nullptr;
            }
    }
}

// Return the contents of the given registers at the given destination
// address
StatusCode KTF11_A::read (u16 address, u16 *destination)
{
    u16* regPtr = registerPointer (address);
    if (regPtr != nullptr)
    {
        *destination = *regPtr;
        return StatusCode::OK;
    }

    return StatusCode::NonExistingMemory;
}

StatusCode KTF11_A::writeWord (u16 address, u16 value)
{
    if (address == statusRegister1 || address == statusRegister2)
        return StatusCode::ReadOnly;

    u16* regPtr = registerPointer (address);
    if (regPtr != nullptr)
    {
        *regPtr = value;
        return StatusCode::OK;
    }

    return StatusCode::NonExistingMemory;
}

bool KTF11_A::responsible (u16 address)
{
    return registerPointer (address) != nullptr;
}

void KTF11_A::reset ()
{}

#endif // _KTF11AREGISTERS_H_