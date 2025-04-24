#include "kdf11_u.h"
#include "bus/qbus/qbus.h"
#include "configdata/serialconfig/uarttypeconfig/uarttypeconfig.h"

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

KDF11_U::KDF11_U (Bus* bus, shared_ptr<KDF11_UConfig> kdf11_uConfig)
    :
    bus_ {bus},
    startAddress_ {kdf11_uConfig->bootAddress}
{
    // The KDF11-U comes with the KTF11-A MMU and two serial lines
    serialLineUnits = make_unique<SerialLineUnits> (bus,
        UARTTypeConfig {.maintenanceModeSupported = true},
        (SLUConfig*) kdf11_uConfig->sluConfig.get());

    vector<BusDevice*> devices {&cpuData_, &mmu_, serialLineUnits.get (),
        &displayRegister_};
    registerHandler_ = make_unique<RegisterHandler> (devices);

    // Besides a pointer to the bus, a reference to our cpu, the start address
    // and the power-up mode, the ControlLogic also gets passed a
    // std::function to the function to create ODT objects. The last parameter
    // to that function indicates if the ODT HALT command is supported.
    controlLogic_ = make_unique<ControlLogic> (bus_,
        &cpuData_,
        &cpuControl_,
        &mmu_,
        kdf11_uConfig->powerUpMode,
        startAddress_,
        bind (&KDF11_ODT::createODT, _1, _2, _3, _4, _5, true));
}

KDF11_U::~KDF11_U ()
{
    controlLogic_->exit ();
    if (kd11Thread_.joinable ())
        kd11Thread_.join ();
}

void KDF11_U::start ()
{
    kd11Thread_ = thread ([&, this] {controlLogic_->run ();});
}

// Start the ControlLogic state machine, starting the CPU at the given address. This
// address supersedes the standard boot address.
void KDF11_U::start (u16 startAddress)
{
    startAddress_ = startAddress;
    start ();
}

// Registers accesses are forwarded to the register handler.
//
CondData<u16> KDF11_U::read (BusAddress address)
{
    return registerHandler_->read (address);
}

StatusCode KDF11_U::writeWord (BusAddress address, u16 value)
{
    return registerHandler_->writeWord (address, value);
}

bool KDF11_U::responsible (BusAddress address)
{
    return registerHandler_->responsible (address);
}

void KDF11_U::reset ()
{
    return registerHandler_->reset ();
}
