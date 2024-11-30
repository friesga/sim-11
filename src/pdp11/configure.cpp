#include "pdp11.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/rxv21/rxv21config/rxv21config.h"
#include "configdata/msv11config/msv11config.h"
#include "configdata/bdv11config/bdv11config.h"
#include "configdata/kdf11/kdf11_a/kdf11_aconfig/kdf11_aconfig.h"
#include "configdata/kdf11/kdf11_b/kdf11_bconfig/kdf11_bconfig.h"
#include "configdata/kdf11/kdf11_u/kdf11_uconfig/kdf11_uconfig.h"
#include "configdata/consistencychecker/consistencychecker.h"
#include "console/operatorconsole/operatorconsolefactory.h"
#include "cabinet/cabinet.h"
#include "overloaded.h"

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

    // Create a DLV11J object with the default configuration. The default
    // position for the BA11-N is at the top of the cabinet (which is 20 RU
    // high, racks units numbered 0-19).
    busDevices_.emplace_back (new DLV11J (&bus_, make_shared<DLV11JConfig> ()));
    busDevices_.emplace_back (new BDV11 (&bus_));
    busDevices_.emplace_back (new RXV21 (&bus_));
    busDevices_.emplace_back (new RLV12 (&bus_));
    ba11_n_ = std::make_unique<BA11_N> (&bus_, window, 
        make_shared<BA11_NConfig> (Cabinet::Position {0, 19_ru}));

    installModules ();
    reset ();
}

// Configure the devices and install them on the bus.
// Accessing a non-configured device will result in a bus time-out
// and the BDV11 boot will halt at address 000010.
void PDP_11::configureDevices (SystemConfig systemConfig,
    Window *window)
{
    // Check for presence of necessary devices
    ConsistencyChecker consistencyChecker {systemConfig};
    consistencyChecker.checkAll ();

    if (systemConfig.isQbusSystem ())
        configureQbusSystem (systemConfig, window);
    else
        configureUnibusSystem (systemConfig, window);

    installModules ();
    reset ();
}

void PDP_11::configureQbusSystem (SystemConfig systemConfig,
    Window* window)
{ 
    // At this point a Qbus system configuration should not contain
    // any Unibus device.
    auto configVisitor = overloaded
    {
        [this] (shared_ptr<KD11_NAConfig> kd11_naConfig)
            {processor_ = new KD11_NA (&bus_, kd11_naConfig); },
        [this] (shared_ptr<KDF11_AConfig> kdf11_aConfig)
            {processor_ = new KDF11_A (&bus_, kdf11_aConfig); },
        [this] (shared_ptr<KDF11_BConfig> kdf11_bConfig)
            {processor_ = new KDF11_B (&bus_, kdf11_bConfig); },
        [this] (shared_ptr<MSV11Config> msv11Config)
            {msv11_.push_back (new MSV11D (&bus_, msv11Config)); },
        [this] (shared_ptr<DLV11JConfig> dlv11jConfig)
            {busDevices_.emplace_back (new DLV11J (&bus_, dlv11jConfig)); },
        [this] (shared_ptr<BDV11Config> bdv11Config)
            {busDevices_.emplace_back (new BDV11 (&bus_, bdv11Config)); },
        [this] (shared_ptr<RXV21Config> rxv21Config)
            {busDevices_.emplace_back (new RXV21 (&bus_, rxv21Config)); },
        [this, window] (shared_ptr<RLConfig> rlConfig)
            {busDevices_.emplace_back (new RLV12 (&bus_, window, rlConfig)); },
        [this, window] (shared_ptr<BA11_NConfig> ba11_nConfig)
            {ba11_n_ = std::make_unique<BA11_N> (&bus_, window, ba11_nConfig); },
        [this] (shared_ptr<MS11PConfig> ms11pConfig)
            { throw "Should not happen"; },
        [this] (shared_ptr<BA11_LConfig> ba11lConfig)
            { throw "Should not happen"; },
        [this] (shared_ptr<KDF11_UConfig> kdf11_uConfig)
            { throw "Should not happen"; }
    };

    for (DeviceConfig deviceConfigVariant : systemConfig)
    {
            visit (configVisitor, deviceConfigVariant);
    }
}

void PDP_11::configureUnibusSystem (SystemConfig systemConfig,
    Window* window)
{
    // At this point a Unibus system configuration should not contain
    // any Qbus device.
    auto configVisitor = overloaded
    {
        [this] (shared_ptr<KD11_NAConfig> kd11_naConfig)
            { throw "Should not happen"; },
        [this] (shared_ptr<KDF11_AConfig> kdf11_aConfig)
            { throw "Should not happen"; },
        [this] (shared_ptr<KDF11_BConfig> kdf11_bConfig)
            { throw "Should not happen"; },
        [this] (shared_ptr<MSV11Config> msv11Config)
            { throw "Should not happen"; },
        [this] (shared_ptr<DLV11JConfig> dlv11jConfig)
            { throw "Should not happen"; },
        [this] (shared_ptr<BDV11Config> bdv11Config)
            { throw "Should not happen"; },
        [this] (shared_ptr<RXV21Config> rxv21Config)
            { throw "Should not happen"; },
        [this, window] (shared_ptr<RLConfig> rlConfig)
            { throw "Should not happen"; },
        [this, window] (shared_ptr<BA11_NConfig> ba11_nConfig)
            { throw "Should not happen"; },
        [this] (shared_ptr<KDF11_UConfig> kdf11_uConfig)
            {processor_ = new KDF11_U (&bus_, kdf11_uConfig); },
        [this] (shared_ptr<MS11PConfig> ms11pConfig)
            {ms11p_.push_back (new MS11P (&bus_, ms11pConfig)); },
        [this, window] (shared_ptr<BA11_LConfig> ba11_lConfig)
            {ba11_l_ = std::make_unique<BA11_L> (&bus_, window, ba11_lConfig); },
    };

    for (DeviceConfig deviceConfigVariant : systemConfig)
    {
        visit (configVisitor, deviceConfigVariant);
    }
}
