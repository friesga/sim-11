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
    bus_ (bus),
    powerUpMode_ {KDF11_AConfig::PowerUpMode::Bootstrap},
    startAddress_ {stdBootAddress}
{
    // Besides a pointer to the bus, a reference to our cpu, the start address
    // and the power-up mode, the ControlLogic also gets passed a
    // std::function to the function to create ODT objects.
    controlLogic_ = make_unique<ControlLogic> (bus_, &cpuData_, &cpu_, &mmu_, powerUpMode_,
        startAddress_, bind (&KD11_NA_ODT::createODT, _1, _2, _3, _4, _5));
}

KDF11_A::KDF11_A (Qbus *bus, shared_ptr<KDF11_AConfig> kdf11_aConfig)
    :
    bus_ (bus),
    powerUpMode_ {kdf11_aConfig->powerUpMode},
    startAddress_ {stdBootAddress}
{
    // Besides a pointer to the bus, a reference to our cpu, the start address
    // and the power-up mode, the ControlLogic also gets passed a
    // std::function to the function to create ODT objects.
    controlLogic_ = make_unique<ControlLogic> (bus_, &cpuData_, &cpu_, &mmu_, powerUpMode_,
        startAddress_, bind (&KD11_NA_ODT::createODT, _1, _2, _3, _4, _5));
}

KDF11_A::~KDF11_A ()
{
    controlLogic_->exit ();
    kd11Thread_.join ();
}

KDF11_A_Cpu& KDF11_A::cpu ()
{
    return cpu_;
}

void KDF11_A::start ()
{
    kd11Thread_ = thread ([&, this] {controlLogic_->run ();});
}

// Start the ControlLogic state machine, starting the CPU at the given address. This
// address supersedes the standard boot address.
void KDF11_A::start (u16 startAddress)
{
    startAddress_ = startAddress;
    start ();
}

// The KDF11-A processor allows access to the PSW via address 0177776. This
// is implemented via a bus access. Micronote 70 states: "When the PSW is
// explicitly addressed (MOV #340, @#177776), the transfer will appear on the
// bus in a similar fashion to the MMU registers except that there will be no
// reply. Likewise, there can be no connnunication from a bus master to the
// PSW", so this implementation is not completely accurate.
//
// Table 8-1 (Processor Status Word Protection) in EK-KDF11-UG-PR2 states
// that on Explicit PS Access the T-bit is unchanged. Therefore the function
// setPSW() can be used to set the PSW to the new value.
//
StatusCode KDF11_A::read (u16 address, u16* destination)
{
    if (address == PSWAddress)
    {
        *destination = cpu_.pswValue ();
        return StatusCode::OK;
    }

    return StatusCode::NonExistingMemory;
}

StatusCode KDF11_A::writeWord (u16 address, u16 value)
{
    if (address == PSWAddress)
    {
        cpu_.setPSW (value);
        return StatusCode::OK;
    }

    return StatusCode::NonExistingMemory;
}

bool KDF11_A::responsible (u16 address)
{
    return (address == PSWAddress) ? true : false; 
}
