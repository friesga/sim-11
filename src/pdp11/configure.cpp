#include "pdp11.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/rxv21config/rxv21config.h"
#include "configdata/msv11config/msv11config.h"
#include "configdata/bdv11config/bdv11config.h"
#include "configdata/kdf11_aconfig/kdf11_aconfig.h"
#include "configdata/kdf11_b/kdf11_bconfig/kdf11_bconfig.h"
#include "console/operatorconsole/operatorconsolefactory.h"

#include <memory>		// For make_unique
#include <cstring>		// For memset()

using std::shared_ptr;
using std::make_shared;
using std::static_pointer_cast;
using std::string;
using std::get;

// Configure the LSI-11 with a default configuration. Create a bare system
// without any files attached.
void PDP_11::configureDevices (Window *window)
{
    // By default use the KD11-NA processor
    processor_ = new KD11_NA (&bus_);
    msv11_.push_back (new MSV11D (&bus_));

    // Create a DLV11J object with the default condiguration
    dlv11_ = new DLV11J (&bus_, make_shared<DLV11JConfig> ());
    bdv11_ = new BDV11 (&bus_);
    rxv21_ = new RXV21 (&bus_);
    rlv12_ = new RLV12 (&bus_);
    ba11_n_ = std::make_unique<BA11_N> (&bus_, window, 
        make_shared<BA11_NConfig> ());

    installModules ();
    reset ();
}

// �Inherit� all function call operators of passed base types
template<typename... Ts> struct overloaded : Ts... { using Ts::operator()...; };

// explicit deduction guide (not needed as of C++20)
// template<typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

// Configure the devices and install them on the bus.
// Accessing a non-configured device will result in a bus time-out
// and the BDV11 boot will halt at address 000010.
void PDP_11::configureDevices (vector<DeviceConfig> systemConfig,
    Window *window)
{
    // Check for presence of necessary devices
    checkConsistency (systemConfig);

    auto configVisitor = overloaded 
    {
        [this] (shared_ptr<KD11_NAConfig> kd11_naConfig)
            {processor_ = new KD11_NA (&bus_, kd11_naConfig); },
        [this] (shared_ptr<KDF11_AConfig> kdf11_aConfig)
            {processor_ = new KDF11_A (&bus_, kdf11_aConfig); },
        [this] (shared_ptr<KDF11_BConfig> kdf11_bConfig)
            {processor_ = new KDF11_B (&bus_, kdf11_bConfig); },
        [this] (shared_ptr<MSV11Config> msv11Config)
            {msv11_.push_back (new MSV11D (&bus_, msv11Config));},
        [this] (shared_ptr<DLV11JConfig> dlv11jConfig)
            {dlv11_ = new DLV11J (&bus_, dlv11jConfig); },
        [this] (shared_ptr<BDV11Config> bdv11Config)
            {bdv11_ = new BDV11 (&bus_, bdv11Config); },
        [this] (shared_ptr<RXV21Config> rxv21Config)
            {rxv21_ = new RXV21 (&bus_, rxv21Config); },
        [this] (shared_ptr<RLConfig> rlConfig)
            {rlv12_ = new RLV12 (&bus_, rlConfig); },
        [this, window] (shared_ptr<BA11_NConfig> ba11_nConfig) 
            {ba11_n_ = std::make_unique<BA11_N> (&bus_, window, ba11_nConfig); },
        [this] (shared_ptr<RXV21UnitConfig> rxv21UnitConfig)
            { throw std::runtime_error ("RXV21UnitConfig in systemConfig"); },
        [this] (shared_ptr<RLUnitConfig> rlUnitConfig)
            { throw std::runtime_error ("RLUnitConfig in systemConfig"); },
        [this] (shared_ptr<SLUConfig> sluConfig)
            { throw std::runtime_error ("SLUConfig in systemConfig"); },
    };

    for (DeviceConfig deviceConfigVariant : systemConfig)
    {
        visit (configVisitor, deviceConfigVariant);
    }

    installModules ();
    reset ();
}
