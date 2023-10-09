#ifndef _KDF11_A_H_
#define _KDF11_A_H_

#include "kd/include/pdp11processor.h"
#include "kd/kdf11_a/cpu/kdf11_a_cpu.h"
#include "kd/kd11_na/odt/kd11_na_odt.h"
#include "configdata/kdf11_aconfig/kdf11_aconfig.h"
#include "kd/common/controllogic/controllogic.h"

#include <memory>

using std::unique_ptr;
using std::shared_ptr;

// The class KDF11_A starts the control logic which on its turn has to run
// the KDF11_A's cpu and start the KD11_FA's ODT.
class KDF11_A : public PDP11Processor
{
public:
    KDF11_A (Qbus* bus);
    KDF11_A (Qbus* bus, shared_ptr<KDF11_AConfig> kdf11_aConfig);
    ~KDF11_A ();
    void start (u16 startAddress);
    void start ();

    // Give main() access to the CPU to set PC and runState
    KDF11_A_Cpu& cpu ();

    // The KDF11_A is a PDP11Peripheral without registers so the read and write 
    // register functions are dummies. The reset function is called on a
    // bus reset and has no function for the KDF11_A either.
    StatusCode read (u16 addr, u16* destination) override
        { return StatusCode::FunctionNotImplemented; };
    StatusCode writeWord (u16 addr, u16 value) override
        { return StatusCode::FunctionNotImplemented; };
    StatusCode writeByte (u16 addr, u8 value) override
        { return StatusCode::FunctionNotImplemented; };
    bool responsible (u16 addr) override
        { return false; }
    void reset () override {};

private:
    enum { stdBootAddress = 0173000 };

    Qbus* bus_;
    KDF11_A_Cpu cpu_ {bus_};
    unique_ptr<KD11_NA_ODT>	odt_ {};
    KDF11_AConfig::PowerUpMode powerUpMode_;
    u16 startAddress_;

    // Besides a pointer to the bus, a reference to our cpu, the start address
    // and the power-up mode, the ControlLogic also gets passed a
    // std::function to the function to create ODT objects.
    ControlLogic controlLogic_ {bus_, &cpu_, powerUpMode_, startAddress_,
        bind (&KD11_NA_ODT::createODT, _1, _2, _3)};

    // The KDF11_A is started in its own thread
    std::thread kd11Thread_;
};

#endif // !_KDF11_A_H_
