#ifndef _RK11D_H_
#define _RK11D_H_

#include "bus/include/bus.h"
#include "configdata/rk/rk11d/rk11dconfig/rk11dconfig.h"
#include "rk/rk05/rk05.h"
#include "rk/include/rkdefinitions.h"
#include "panel.h"
#include "bitfield.h"

#include <vector>
#include <string>
#include <mutex>
#include <queue>
#include <thread>

using std::vector;
using std::unique_ptr;
using std::mutex;
using std::queue;
using std::thread;
using std::condition_variable;

class RK11D : public AbstractBusDevice
{
private:
    // Define RK11-D registers as offsets from the controller's base address
    //
    // Address 777414 is the Maintenance Register in a RK11-C but is unused
    // on the RK11-D.
    //
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
    RKTypes::RKDS rkds_ {0};
    RKTypes::RKER rker_ {0};
    RKTypes::RKCS rkcs_ {0};
    u16 rkwc_ {0};
    u16 rkba_ {0};
    RKTypes::RKDA rkda_ {0};
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

private:
    // Definition of the controller's base address and vector
    u16 baseAddress_ {0};
    u16 vector_ {0};

    // Action processor thread
    bool running_ {false};
    thread actionProcessorThread_;

    // Safe guard against controller access from multiple threads
    mutex controllerMutex_;

    // Definition of the queue for forwarding actions to the action processor.
    // The queue is accessed from multiple threads ans its consistency has to
    // be safe-guarded by the controllerMutex_.
    queue<RKTypes::Function> rk11ActionQueue_;

    // Condition variable to wake up the action processor when an action has
    // been queued.
    condition_variable actionAvailable_;

    void actionProcessor ();
    void processAction (RKTypes::Function action);
    void BINITReceiver (bool signalValue);
    void setNonExistingDisk (u16 driveId);
};


#endif // _RK11D_H_