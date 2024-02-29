#include "kdf11_b.h"
#include "qbus/qbus.h"

#include <memory>
#include <thread>

using std::shared_ptr;
using std::make_unique;
using std::static_pointer_cast;
using std::thread;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;

KDF11_B::KDF11_B (Qbus *bus, shared_ptr<KDF11_BConfig> kdf11_bConfig)
    :
    KDF11 (bus, kdf11_bConfig->powerUpMode, stdBootAddress)
{
    // The KDF11-B comes with the KTF11-A MMU, two serial lines and a BDV11-11
    // compatible module.
    cpuModules_.push_back (&mmu_);

    // Add the serial line units
    serialLineUnits = make_unique<SerialLineUnits> (bus, (SLUConfig*) kdf11_bConfig->sluConfig.get());
    cpuModules_.push_back (serialLineUnits.get ());

    // Add the BDV11-compatible module
    bdv11 = make_unique<BDV11> (bus, static_pointer_cast<BDV11Config> (kdf11_bConfig->bdv11Config));
    cpuModules_.push_back (bdv11.get ());

    // Besides a pointer to the bus, a reference to our cpu, the start address
    // and the power-up mode, the ControlLogic also gets passed a
    // std::function to the function to create ODT objects.
    controlLogic_ = make_unique<ControlLogic> (bus_, &cpuData_, &cpuControl_, &mmu_, powerUpMode_,
        startAddress_, bind (&KDF11_ODT::createODT, _1, _2, _3, _4, _5));
}

