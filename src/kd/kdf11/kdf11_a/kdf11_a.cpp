#include "kdf11_a.h"
#include "bus/qbus/qbus.h"

#include <memory>
#include <thread>
#include <vector>

using std::shared_ptr;
using std::make_unique;
using std::thread;
using std::vector;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;

KDF11_A::KDF11_A (Bus *bus, shared_ptr<KDF11_AConfig> kdf11_aConfig)
    :
    bus_ {bus},
    startAddress_ {kdf11_aConfig->startingAddress}
{
    // If the KTF11-A is configured add it to the CPU modules. If it is not
    // configured its registers will not be available on the bus and
    // consequently it cannot be enabled.
    // 
    // I would have liked to state 'make_unique<RegisterHandler> ({&cpuData_})'
    // but unfortunately make_unique cannot accept an initializer_list.
    vector<BusDevice*> modules {&cpuData_};
    if (kdf11_aConfig->ktf11_a_present)
        modules.push_back (&mmu_);

    registerHandler_ = make_unique<RegisterHandler> (modules);

    // Besides a pointer to the bus, a reference to our cpu, the start address
    // and the power-up mode, the ControlLogic also gets passed a
    // std::function to the function to create ODT objects.  The last parameter
    // to that function indicates if the ODT HALT command is supported.
    controlLogic_ = make_unique<ControlLogic> (bus_,
        &cpuData_,
        &cpuControl_,
        &mmu_,
        kdf11_aConfig->powerUpMode,
        startAddress_,
        bind (&KDF11_ODT::createODT, _1, _2, _3, _4, _5, false));
}

KDF11_A::~KDF11_A ()
{
    controlLogic_->exit ();
    kd11Thread_.join ();
}

void KDF11_A::start ()
{
    kd11Thread_ = thread ([&, this] {controlLogic_->run ();});
}

// Start the ControlLogic state machine, starting the CPU at the given
// address. This address supersedes the standard boot address.
void KDF11_A::start (u16 startAddress)
{
    startAddress_ = startAddress;
    start ();
}

// Registers accesses are forwarded to the register handler.
//
CondData<u16> KDF11_A::read (BusAddress address)
{
    return registerHandler_->read (address);
}

StatusCode KDF11_A::writeWord (BusAddress address, u16 value)
{
    return registerHandler_->writeWord (address, value);
}

bool KDF11_A::responsible (BusAddress address)
{
    return registerHandler_->responsible (address);
}

void KDF11_A::reset ()
{
    return registerHandler_->reset ();
}
