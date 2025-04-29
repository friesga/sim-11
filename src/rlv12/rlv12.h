#ifndef _RLV12_H_
#define _RLV12_H_

#include "rlv12const.h"
#include "trace/trace.h"
#include "rl01_02/rl01_02.h"
#include "bus/include/bus.h"
#include "busaddress/busaddress.h"
#include "abstractbusdevice/abstractbusdevice.h"
#include "threadsafecontainers/threadsafequeue.h"
#include "statuscodes.h"
#include "rlv12command/rlv12command.h"
#include "configdata/rl/rlconfig/rlconfig.h"
#include "cmdprocessor/cmdprocessor.h"

#include <array>
#include <memory>               // for std::unique_ptr<>
#include <thread>               // for std::thread
#include <mutex>                // for std::mutex
#include <condition_variable>   // for std::condition_variable
#include <queue>                // for std::queue

// Forward declaration
class CmdProcessor;

// Implementation of the RL11, RLV11 and RLV12 controllers.
class RLV12 : public AbstractBusDevice
{
    friend class CmdProcessor;

    // Define RLV12 registers as offsets from the controllers base address
    enum
    {
        CSR = 00,
        BAR = 02,
        DAR = 04,
        MPR = 06,
        BAE = 010
    };

    // Define controller registers
    // The MPR is not a single physical entity like the other registers. It
    // consists of two separate registers, the word counter and the FIFO output
    // buffer, both bearing the same base address. (EK-RLV12-TD-001 pg. 3-4)
    u16 csr_;       // Control/status register
    u16 bar_;       // Bus Address register
    u16 dar_;       // Disk address register
    u16 bae_;       // Bus Address Extension register

    // Definition of the controller's base address and vector
    u16 baseAddress_ {0};
    u16 vector_ {0};

    // Pointer to the bus we are connected to
    Bus* bus_;

    // Define transfer buffer
    u16* dataBuffer_;

    // Definition of the controller type and the presence of the 22-bit option
    // when configured as a RLV12 controller.
    RLV12const::RLType rlType_;
    bool _22bit_;

    // A RLV12 can have a maximum of four units. RL01_02 objects for
    // which no unit is connected to the controller will have the status
    // off-line.
    std::array<RL01_02, RLV12const::maxDrivesPerController> units_
    {
        RL01_02 (this),
        RL01_02 (this),
        RL01_02 (this),
        RL01_02 (this)
    };

    // Definition of the pointer to the command processor
    std::unique_ptr<CmdProcessor> cmdProcessor_;

    // Definition of a queue for commands to the command processor
    std::queue<RLV12Command> commandQueue_ {};

    // Safe guard against controller access from multiple service threads
    std::mutex controllerMutex_;

    // Condition variable to wake up the command processor
    std::condition_variable signal_;

    // The MPR consists of two separate registers, the word counter and the
    // FIFO output buffer, both bearing the same base address. The word
    // counter is a write-only register, while the output buffer is a read-only
    // register.
    // When writing the MPR, the data word is loaded into the word counter (WC)
    // register. [...] When reading the MPR, the data word is read from the
    // FIFO output buffer. (EK-RLV-12-TD-001, pg. 4-3)

    // Word counter
    u16 wordCounter_;

    // Index into the FIFO buffer for reading the FIFO via the MPR
    size_t fifoIndex_;

    // Private functions
    u16 calcCRC (int const wc, u16 const* data);
    void setDone (RL01_02& unit, u16 status);
    u16 getStatusCmd (RL01_02* unit);

public:
    // Constructors/destructor
    RLV12 (Bus* bus);
    RLV12 (Bus* bus, Window* window, RLConfig& rlConfig);
    RLV12 (Bus* bus, Window* window, shared_ptr<RL11Config> rl11Config);
    RLV12 (Bus* bus, Window* window, shared_ptr<RLV11Config> rlv11Config);
    RLV12 (Bus* bus, Window* window, shared_ptr<RLV12Config> rlv12Config);
    ~RLV12 ();

    // Required functions
    void reset () override;
    bool responsible (BusAddress addr) override;
    CondData<u16> read (BusAddress busAddress) override;
    StatusCode writeByte (BusAddress busAddress, u8 data) override;
    StatusCode writeWord (BusAddress busAddress, u16 data) override;

    // Declare the signal receivers
    void BINITReceiver (bool signalValue);

    // RLV1[12] specific function
    inline size_t numUnits ();
    inline RL01_02* unit (size_t unitNumber);

    // Functions to set and get memory adresses consistently for
    // 16-, 18- and 22-bit systems
    void memAddrToRegs (u32 memoryAddress);
    BusAddress memAddrFromRegs ();
    void updateBAE ();
    u16 rlcsPlusDriveStatus (RL01_02& unit);
    constexpr u16 getBA16BA17 (u16 csr_);
};


inline size_t RLV12::numUnits ()
{
    return RLV12const::maxDrivesPerController;
}

inline RL01_02* RLV12::unit (size_t unitNumber)
{
    return &units_[unitNumber];
}

#endif // _RLV12_H_