#include "pdp11.h"
#include "bus/qbus/qbus.h"
#include "bus/unibus/unibus.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/rxv21/rxv21config/rxv21config.h"
#include "configdata/msv11config/msv11config.h"
#include "configdata/mm11_econfig/mm11_econfig.h"
#include "configdata/bdv11config/bdv11config.h"
#include "configdata/ka11/ka11config/ka11config.h"
#include "configdata/kdf11/kdf11_a/kdf11_aconfig/kdf11_aconfig.h"
#include "configdata/kdf11/kdf11_b/kdf11_bconfig/kdf11_bconfig.h"
#include "configdata/kdf11/kdf11_u/kdf11_uconfig/kdf11_uconfig.h"
#include "configdata/kt24/kt24config/kt24config.h"
#include "configdata/m792/m792config/m792config.h"
#include "configdata/m9312/m9312config/m9312config.h"
#include "configdata/consistencychecker/consistencychecker.h"
#include "console/operatorconsole/operatorconsolefactory.h"
#include "cabinet/cabinet.h"
#include "overloaded.h"

#include <cstring>		// For memset()

using std::string;
using std::get;

// Configure the system with a default configuration. Create a bare system
// without any files attached.
//
void PDP_11::configureDevices (Window *window, AudioSystem* audioSystem)
{
    // Create the Qbus to be used in this system
    bus_ = new Qbus ();

    // By default use the KD11-NA processor
    processor_ = new KD11_NA (bus_);
    memoryDevices_.push_back (new MSV11D (bus_));

    // Create a DLV11J object with the default configuration. The default
    // position for the BA11-N is at the top of the cabinet (which is 20 RU
    // high, racks units numbered 0-19).
    busDevices_.emplace_back (new DLV11J (bus_, DLV11JConfig {}));
    busDevices_.emplace_back (new BDV11 (bus_));
    busDevices_.emplace_back (new RXV21 (bus_));
    busDevices_.emplace_back (new RLV12 (bus_));
    ba11_n_ = std::make_unique<BA11_N> (bus_, window, 
        BA11_NConfig {Cabinet::Position {0, 19_ru}});

    installModules ();
    reset ();
}

// Configure the devices and install them on the bus.
// Accessing a non-configured device will result in a bus time-out
// and the BDV11 boot will halt at address 000010.
void PDP_11::configureDevices (SystemConfig const & systemConfig,
    Window *window, AudioSystem* audioSystem)
{
    // Check for presence of necessary devices
    ConsistencyChecker consistencyChecker {systemConfig};
    consistencyChecker.checkAll ();

    if (systemConfig.isQbusSystem ())
        configureQbusSystem (systemConfig, window, audioSystem);
    else
        configureUnibusSystem (systemConfig, window, audioSystem);

    installModules ();
    reset ();
}

void PDP_11::configureQbusSystem (SystemConfig const & systemConfig,
    Window* window, AudioSystem* audioSystem)
{ 
    // Create the Qbus to be used in this system
    bus_ = new Qbus ();

    // At this point a Qbus system configuration should not contain
    // any Unibus device.
    auto configVisitor = overloaded
    {
        [this] (KD11_NAConfig kd11_naConfig)
            {processor_ = new KD11_NA (bus_, kd11_naConfig); },
        [this] (KDF11_AConfig kdf11_aConfig)
            {processor_ = new KDF11_A (bus_, kdf11_aConfig); },
        [this] (KDF11_BConfig kdf11_bConfig)
            {processor_ = new KDF11_B (bus_, kdf11_bConfig); },
        [this] (MSV11Config msv11Config)
            {memoryDevices_.push_back (new MSV11D (bus_, msv11Config)); },
        [this] (MM11EConfig mm11eConfig)
            {throw logic_error ("Should not happen"); },
        [this] (DLV11JConfig dlv11jConfig)
            {busDevices_.emplace_back (new DLV11J (bus_, dlv11jConfig)); },
        [this] (BDV11Config bdv11Config)
            {busDevices_.emplace_back (new BDV11 (bus_, bdv11Config)); },
        [this] (RXV21Config rxv21Config)
            {busDevices_.emplace_back (new RXV21 (bus_, rxv21Config)); },
        [this, window] (RK11DConfig rk11dConfig)
            {throw logic_error ("Should not happen"); },
        [this, window] (RL11Config rl11Config)
            {throw logic_error ("Should not happen"); },
        [this, window] (RLV11Config rlv11Config)
            {busDevices_.emplace_back (new RLV12 (bus_, window, rlv11Config)); },
        [this, window] (RLV12Config rlv12Config)
            {busDevices_.emplace_back (new RLV12 (bus_, window, rlv12Config)); },
        [this, window] (BA11_NConfig ba11_nConfig)
            {ba11_n_ = std::make_unique<BA11_N> (bus_, window, ba11_nConfig); },
        [this] (KT24Config kt24Config)
            { throw logic_error ("Should not happen"); },
        [this] (M9312Config m9312Config)
            { throw logic_error ("Should not happen"); },
        [this] (MS11PConfig ms11pConfig)
            { throw logic_error ("Should not happen"); },
        [this] (BA11_LConfig ba11lConfig)
            { throw logic_error ("Should not happen"); },
        [this] (KA11Config ka11Config)
            { throw logic_error ("Should not happen"); },
        [this] (KDF11_UConfig kdf11_uConfig)
            { throw logic_error ("Should not happen"); },
        [this] (DL11Config dl11Config)
            { throw logic_error ("Should not happen"); },
        [this] (M792Config m792Config)
            { throw logic_error ("Should not happen"); },
        [this] (KW11LConfig kw11lConfig)
            { throw logic_error ("Should not happen"); }
    };

    for (DeviceConfig deviceConfigVariant : systemConfig)
    {
            visit (configVisitor, deviceConfigVariant);
    }
}

void PDP_11::configureUnibusSystem (SystemConfig const & systemConfig,
    Window* window, AudioSystem* audioSystem)
{
    // Create the Unibus to be used in this system
    bus_ = new Unibus ();

    // At this point a Unibus system configuration should not contain
    // any Qbus device.
    auto configVisitor = overloaded
    {
        [this] (KD11_NAConfig kd11_naConfig)
            { throw logic_error ("Should not happen"); },
        [this] (KDF11_AConfig kdf11_aConfig)
            { throw logic_error ("Should not happen"); },
        [this] (KDF11_BConfig kdf11_bConfig)
            { throw logic_error ("Should not happen"); },
        [this] (MSV11Config msv11Config)
            { throw logic_error ("Should not happen"); },
        [this] (MM11EConfig mm11eConfig)
            {memoryDevices_.push_back (new MM11E (bus_, mm11eConfig)); },
        [this] (DLV11JConfig dlv11jConfig)
            { throw logic_error ("Should not happen"); },
        [this] (BDV11Config bdv11Config)
            { throw logic_error ("Should not happen"); },
        [this] (RXV21Config rxv21Config)
            { throw logic_error ("Should not happen"); },
        [this, window, audioSystem] (RK11DConfig rk11dConfig)
            { busDevices_.emplace_back (new RK11D (bus_, window, audioSystem, rk11dConfig)); },
        [this, window] (RL11Config rl11Config)
            { busDevices_.emplace_back (new RLV12 (bus_, window, rl11Config)); },
        [this, window] (KT24Config kt24Config)
            { kt24_ = new KT24 (bus_, kt24Config); },
        [this, window] (M9312Config m9312Config)
            { m9312_ = new M9312 (bus_, m9312Config); },
        [this, window] (RLV11Config rlConfig)
            { throw logic_error ("Should not happen"); },
        [this, window] (RLV12Config rlConfig)
            { throw logic_error ("Should not happen"); },
        [this, window] (BA11_NConfig ba11_nConfig)
            { throw logic_error ("Should not happen"); },
        [this, window, audioSystem] (KA11Config ka11Config)
            { processor_ = new KA11 (bus_, window, audioSystem, ka11Config); },
        [this] (KDF11_UConfig kdf11_uConfig)
            {processor_ = new KDF11_U (bus_, kdf11_uConfig); },
        [this] (MS11PConfig ms11pConfig)
            {memoryDevices_.push_back (new MS11P (bus_, ms11pConfig)); },
        [this, window] (BA11_LConfig ba11_lConfig)
            {ba11_l_ = std::make_unique<BA11_L> (bus_, window, ba11_lConfig); },
        [this] (DL11Config dl11Config)
            { busDevices_.emplace_back (new DL11 (bus_, dl11Config)); },
        [this] (M792Config m792Config)
            {  busDevices_.emplace_back (new M792 (bus_, m792Config)); },
        [this] (KW11LConfig kw11lConfig)
            { busDevices_.emplace_back (new KW11L (bus_, kw11lConfig)); }
    };

    for (DeviceConfig deviceConfigVariant : systemConfig)
    {
        visit (configVisitor, deviceConfigVariant);
    }
}
