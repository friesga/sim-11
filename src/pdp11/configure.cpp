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

// Configure the devices and install them on the bus.
// Accessing a non-configured device will result in a bus time-out
// and the BDV11 boot will halt at address 000010.
void PDP_11::configureDevices (vector<DeviceConfigVariant> systemConfig,
    Window *window)
{
    // Check for presence of necessary devices
    checkConsistency (systemConfig);

    // Get the device configurations and populate the LSI bus with these devices.
    for (auto device : systemConfig)
    {
        switch (device.index ())
        {
            // case DeviceType::KD11_NA:
            case 0:
                processor_ = new KD11_NA (&bus_,
                    get<shared_ptr<KD11_NAConfig>> (device));
                break;

            // case DeviceType::KDF11_A:
            case 1:
                processor_ = new KDF11_A (&bus_,
                    get<shared_ptr<KDF11_AConfig>> (device));
                break;

            // case DeviceType::KDF11_B:
            case 2:
                processor_ = new KDF11_B (&bus_,
                    get<shared_ptr<KDF11_BConfig>> (device));
                break;

            // case DeviceType::MSV11:
            case 3:
                msv11_.push_back (new MSV11D (&bus_,
                    get<shared_ptr<MSV11Config>> (device)));
                break;

            // case DeviceType::DLV11_J:
            case 4:
                dlv11_ = new DLV11J (&bus_, 
                    get<shared_ptr<DLV11JConfig>> (device));
                break;

            // case DeviceType::BDV11:
            case 5:
                bdv11_ = new BDV11 (&bus_,
                    get<shared_ptr<BDV11Config>> (device));
                break;

            // case DeviceType::RXV21:
            case 6:
                rxv21_ = new RXV21 (&bus_,
                    get<shared_ptr<RXV21Config>> (device));
                break;

            // case DeviceType::RLV12:
            case 7:
                rlv12_ = new RLV12
                    (&bus_, get<shared_ptr<RLConfig>> (device));
                break;

            // case DeviceType::BA11_N:
            case 8:
                ba11_n_ = std::make_unique<BA11_N> (&bus_, window, 
                    get<shared_ptr<BA11_NConfig>> (device));
                break;

            default:
                throw "Unknown device type in configuration";
        }
    }

    installModules ();
    reset ();
}
