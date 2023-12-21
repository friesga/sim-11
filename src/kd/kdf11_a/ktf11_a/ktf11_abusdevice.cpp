#ifndef _KTF11AREGISTERS_H_
#define _KTF11AREGISTERS_H_

#include "ktf11_a.h"
#include "kd/include/psw.h"
#include "basicregister/readonlyregister.h"
#include "trace/trace.h"

Register* KTF11_A::registerPointer (u16 address)
{
    switch (address)
    {
        case statusRegister0:
            return &sr0_;

        case statusRegister1:
            return &readOnlySr1_;

        case statusRegister2:
            return &readOnlySr2_;

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
                    return &activePageRegisterSet_[static_cast<u16> (PSW::Mode::Kernel)]
                        .activePageRegister_[index]
                        .pageAddressRegister_;

                case kernelPDRBase:
                    return &activePageRegisterSet_[static_cast<u16> (PSW::Mode::Kernel)]
                        .activePageRegister_[index]
                        .pageDescripterRegister_;

                case userPARBase:
                    return &activePageRegisterSet_[static_cast<u16> (PSW::Mode::User)]
                        .activePageRegister_[index]
                        .pageAddressRegister_;

                case userPDRBase:
                    return &activePageRegisterSet_[static_cast<u16> (PSW::Mode::User)]
                        .activePageRegister_[index]
                        .pageDescripterRegister_;

                default:
                    return nullptr;
            }
    }
}

// Return the contents of the given registers at the given destination
// address
StatusCode KTF11_A::read (u16 address, u16 *destination)
{
    Register* regPtr = registerPointer (address);
    if (regPtr != nullptr)
    {
        *destination = *regPtr;
        return StatusCode::OK;
    }

    return StatusCode::NonExistingMemory;
}

StatusCode KTF11_A::writeWord (u16 address, u16 value)
{
    Register* regPtr = registerPointer (address);
    if (regPtr != nullptr)
    {
        *regPtr = value;
        trace.MmuApr (activePageRegisterSet_[static_cast<u16> (PSW::Mode::Kernel)]);
        return StatusCode::OK;
    }

    return StatusCode::NonExistingMemory;
}

bool KTF11_A::responsible (u16 address)
{
    return registerPointer (address) != nullptr;
}

// SR0 and SR3 are cleared by the RESET instruction, power-up or restart. 
void KTF11_A::reset ()
{
    sr0_ = 0;
    sr3_ = 0;
}

#endif // _KTF11AREGISTERS_H_