#include "kdf11_b.h"
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

KDF11_B::KDF11_B (Bus *bus, shared_ptr<KDF11_BConfig> kdf11_bConfig)
    :
    bus_ {bus},
    startAddress_ {stdBootAddress}
{
    // The KDF11-B comes with the KTF11-A MMU, two serial lines and a BDV-11.
    serialLineUnits = make_unique<SerialLineUnits> (bus,
        UARTTypeConfig {.maintenanceModeSupported = false},
        (SLUConfig*) kdf11_bConfig->sluConfig.get());
    bdv11 = make_unique<BDV11> (bus,
        static_pointer_cast<BDV11Config> (kdf11_bConfig->bdv11Config));

    vector<BusDevice*> devices {&cpuData_, &mmu_, serialLineUnits.get (), bdv11.get ()};
    registerHandler_ = make_unique<RegisterHandler> (devices);

    // Besides a pointer to the bus, a reference to our cpu, the start address
    // and the power-up mode, the ControlLogic also gets passed a
    // std::function to the function to create ODT objects.
    controlLogic_ = make_unique<ControlLogic> (bus_,
        &cpuData_,
        &cpuControl_,
        &mmu_,
        kdf11_bConfig->powerUpMode,
        startAddress_,
        bind (&KDF11_ODT::createODT, _1, _2, _3, _4, _5));
}

KDF11_B::~KDF11_B ()
{
    controlLogic_->exit ();
    kd11Thread_.join ();
}

void KDF11_B::start ()
{
    kd11Thread_ = thread ([&, this] {controlLogic_->run ();});
}

// Start the ControlLogic state machine, starting the CPU at the given address. This
// address supersedes the standard boot address.
void KDF11_B::start (u16 startAddress)
{
    startAddress_ = startAddress;
    start ();
}

// Registers accesses are forwarded to the register handler.
//
StatusCode KDF11_B::read (BusAddress address, u16* destination)
{
    return registerHandler_->read (address, destination);
}

StatusCode KDF11_B::writeWord (BusAddress address, u16 value)
{
    return registerHandler_->writeWord (address, value);
}

bool KDF11_B::responsible (BusAddress address)
{
    return registerHandler_->responsible (address);
}

void KDF11_B::reset ()
{
    return registerHandler_->reset ();
}

