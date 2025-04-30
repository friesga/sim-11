#ifndef _RK11D_H_
#define _RK11D_H_

#include "bus/include/bus.h"
#include "rk/include/rk11.h"
#include "configdata/rk/rk11d/rk11dconfig/rk11dconfig.h"
#include "rk/rk05/rk05.h"
#include "rk/include/rkdefinitions.h"
#include "panel.h"
#include "bitfield.h"

#include <vector>
#include <string>
#include <mutex>

using std::vector;
using std::unique_ptr;
using std::mutex;

class RK11D : public RK11
{
private:
    // Define RK11-D registers as offsets from the controller's base address
    //
    // Address 777414 is unused.
    enum
    {
        RKDS = 00,      // Drive Status register
        RKER = 02,      // Error register
        RKCS = 04,      // Control Status register
        RKWC = 06,      // Word Count register
        RKBA = 010,     // Current Bus Address register
        RKDA = 012,     // Disk Address register
        RKDB = 016,     // Data Buffer register
    };

    // Definition of the controller's registers
    RKDefinitions::RKDS rkds_ {0};
    RKDefinitions::RKER rker_ {0};
    RKDefinitions::RKCS rkcs_ {0};
    u16 rkwc_ {0};
    u16 rkba_ {0};
    RKDefinitions::RKDA rkda_ {0};
    u16 rkdb_ {0};

    // RKCS writable bits mask
    static const u16 rkcsWritableBits {06577};

    // Definition of (pointers to) the attached RK05 drives
    vector<unique_ptr<RK05>> rk05Drives_ {};

    Bus* bus_ {nullptr};

public:
    // Constructor
    RK11D (Bus* bus, Window* window, shared_ptr<RK11DConfig> rk11dConfig);

    // Functions required by the BusDevice interface
    CondData<u16> read (BusAddress busAddress) override;
    StatusCode writeWord (BusAddress busAddress, u16 value) override;
    bool responsible (BusAddress busAddress) override;
    void reset () override;

    // Functions required by the RK11 interface
    void processResult (RKDefinitions::Result result) override;

private:
    // Definition of the controller's base address and vector
    u16 baseAddress_ {0};
    u16 vector_ {0};

    // Safe guard against controller access from multiple threads
    mutex controllerMutex_;

    void BINITReceiver (bool signalValue);
    void processFunction (RKDefinitions::RKCommand command);
};


#endif // _RK11D_H_