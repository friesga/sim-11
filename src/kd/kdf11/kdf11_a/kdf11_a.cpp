#include "kdf11_a.h"
#include "qbus/qbus.h"

#include <memory>
#include <thread>

using std::shared_ptr;
using std::make_unique;
using std::thread;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;

// The factory power-up mode configuration is mode 0 (get vector at address
// 24 and 26), but we'll set it to Bootstrap as that's more convenient for
// the user.
KDF11_A::KDF11_A (Qbus* bus)
    :
    KDF11 (bus, KD11Config::PowerUpMode::Bootstrap, stdBootAddress)
{
    // Add the MMU to the CPU modules
    cpuModules_.push_back (&mmu_);

    // Besides a pointer to the bus, a reference to our cpu, the start address
    // and the power-up mode, the ControlLogic also gets passed a
    // std::function to the function to create ODT objects.
    controlLogic_ = make_unique<ControlLogic> (bus_, &cpuData_, &cpuControl_, &mmu_, powerUpMode_,
        startAddress_, bind (&KDF11_A_ODT::createODT, _1, _2, _3, _4, _5));
}

KDF11_A::KDF11_A (Qbus *bus, shared_ptr<KDF11_AConfig> kdf11_aConfig)
    :
    KDF11 (bus, KD11Config::PowerUpMode::Bootstrap, stdBootAddress)
{
    // If the KTF11-A is configured add it to the CPU modules. If it is not
    // configured its registers will not be available on the bus and
    // consequently it cannot be enabled.
    if (kdf11_aConfig->ktf11_a_present)
        cpuModules_.push_back (&mmu_);

    // Besides a pointer to the bus, a reference to our cpu, the start address
    // and the power-up mode, the ControlLogic also gets passed a
    // std::function to the function to create ODT objects.
    controlLogic_ = make_unique<ControlLogic> (bus_, &cpuData_, &cpuControl_, &mmu_, powerUpMode_,
        startAddress_, bind (&KDF11_A_ODT::createODT, _1, _2, _3, _4, _5));
}


   
