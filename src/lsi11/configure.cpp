#include "lsi11.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/rxv21config/rxv21config.h"
#include "configdata/msv11config/msv11config.h"
#include "configdata/bdv11config/bdv11config.h"
#include "configdata/kdf11_aconfig/kdf11_aconfig.h"
#include "console/operatorconsole/operatorconsolefactory.h"

#include <memory>		// For make_unique
#include <cstring>		// For memset()

using std::shared_ptr;
using std::make_shared;
using std::static_pointer_cast;
using std::string;

using namespace kd11_na;

// Configure the LSI-11 with a default configuration. Create a bare system
// without any files attached.
void LSI11::configureDevices (Window *window)
{
    // The Console class reads characters and sends them to the dlv11
    unique_ptr<Console> console =  OperatorConsoleFactory::create ();

    // By default use the KD11-NA processor
    kd11_na_ = new KD11_NA (&bus_);
    msv11_ = new MSV11D (&bus_);
    dlv11_ = new DLV11J (&bus_, move (console));
    bdv11_ = new BDV11 (&bus_);
    rxv21_ = new RXV21 (&bus_);
    rlv12_ = new RLV12 (&bus_);
    ba11_n_ = std::make_unique<BA11_N> (&bus_, window);

    installModules ();
    reset ();
}

// Configure the devices and install them on the bus.
// Accessing a non-configured device will result in a bus time-out
// and the BDV11 boot will halt at address 000010.
void LSI11::configureDevices (vector<shared_ptr<DeviceConfig>> systemConfig,
    Window *window)
{
    // Check for presence of necessary devices
    checkConsistency (systemConfig);

    // Get the device configurations and populate the LSI bus with these devices.
    for (shared_ptr<DeviceConfig> device : systemConfig)
    {
        switch (device->deviceType_)
        {
            case DeviceType::KD11_NA:
                kd11_na_ = new KD11_NA (&bus_,
                    static_pointer_cast<KD11_NAConfig> (device));
                break;

            case DeviceType::KDF11_A:
                kdf11_a_ = new KDF11_A (&bus_,
                    static_pointer_cast<KDF11_AConfig> (device));
                break;

            case DeviceType::MSV11:
                msv11_ = new MSV11D (&bus_,
                    static_pointer_cast<MSV11Config> (device));
                break;

            case DeviceType::DLV11_J:
                // The Console class reads characters and sends them to the dlv11
                dlv11_ = new DLV11J (&bus_, 
                    move (OperatorConsoleFactory::create ()),
                    static_pointer_cast<DLV11Config> (device));
                break;

            case DeviceType::BDV11:
                bdv11_ = new BDV11 (&bus_,
                    static_pointer_cast<BDV11Config> (device));
                break;

            case DeviceType::RXV21:
                rxv21_ = new RXV21 (&bus_,
                    static_pointer_cast<RXV21Config> (device));
                break;

            case DeviceType::RLV12:
                rlv12_ = new RLV12
                (&bus_, static_pointer_cast<RLConfig> (device));
                break;

            case DeviceType::BA11_N:
                ba11_n_ = std::make_unique<BA11_N> (&bus_, window);
                break;

            default:
                throw "Unknown device type in configuration";
        }
    }

    installModules ();
    reset ();
}
