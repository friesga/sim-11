#include "lsi11.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/rxv21config/rxv21config.h"

#include <memory>		// For make_unique
#include <cstring>		// For memset()

using std::shared_ptr;
using std::make_shared;
using std::static_pointer_cast;
using std::string;

// Configure the LSI-11 with a default configuration. Create a bare system
// without any files attached.
void LSI11::configureDevices ()
{
    kd11_ = new KD11 (&bus_);
    msv11_ = new MSV11D (&bus_);
    dlv11_ = new DLV11J (&bus_);
    bdv11_ = new BDV11 (&bus_);
    rxv21_ = new RXV21 (&bus_);
    rlv12_ = new RLV12 (&bus_);
    ba11_n_ = std::make_unique<BA11_N> (&bus_);

    installModules ();
    reset ();
}

// Configure the devices and install them on the bus.
// Accessing a non-configured device will result in a bus time-out
// and the BDV11 boot will halt at address 000010.
void LSI11::configureDevices (vector<shared_ptr<DeviceConfig>> systemConfig)
{
    // Check for presence of necessary devices
    checkConsistency (systemConfig);

    // Get the device configurations and populate the LSI bus with these devices.
    for (shared_ptr<DeviceConfig> device : systemConfig)
    {
        switch (device->deviceType_)
        {
            case DeviceType::KD11:
                kd11_ = new KD11 (&bus_);
                break;

            case DeviceType::MSV11:
                msv11_ = new MSV11D (&bus_);
                break;

            case DeviceType::DLV11_J:
                dlv11_ = new DLV11J (&bus_, 
                    static_pointer_cast<DLV11Config> (device));
                break;

            case DeviceType::BDV11:
                bdv11_ = new BDV11 (&bus_);
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
                ba11_n_ = std::make_unique<BA11_N> (&bus_);
                break;

            default:
                throw "Unknown device type in configuration";
        }
    }

    installModules ();
    reset ();
}
