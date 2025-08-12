#ifndef _RK11D_H_
#define _RK11D_H_

#include "bus/include/bus.h"
#include "configdata/rk/rk11d/rk11dconfig/rk11dconfig.h"
#include "rk/rk05/rk05.h"
#include "rk/include/rktypes.h"
#include "rk/include/driveinterface.h"
#include "panel.h"
#include "bitfield.h"
#include "threadsafecontainers/threadsafequeue.h"
#include "variantfsm/fsm.h"
#include "statuscodes.h"

#include <vector>
#include <string>
#include <mutex>
#include <queue>
#include <thread>
#include <functional>
#include <semaphore>

using std::vector;
using std::unique_ptr;
using std::mutex;
using std::queue;
using std::thread;
using std::condition_variable;
using std::function;
using std::monostate;
using std::binary_semaphore;

class RK11D : public AbstractBusDevice, public DriveInterface
{
private:
    // Define RK11-D registers as offsets from the controller's base address.
    //
    // Address 777414 is the Maintenance Register in a RK11-C but is unused
    // on the RK11-D. The registers reads as 0, and writes to it are discarded.
    // (https://gunkies.org/wiki/RK11_disk_controller#777414:_Maintenance_Register_.28RKMR.29)
    //
    enum
    {
        RKDS = 00,      // Drive Status register
        RKER = 02,      // Error register
        RKCS = 04,      // Control Status register
        RKWC = 06,      // Word Count register
        RKBA = 010,     // Current Bus Address register
        RKDA = 012,     // Disk Address register
        RKMR = 014,     // Maintenance register
        RKDB = 016,     // Data Buffer register
    };

    // Definition of the controller's registers. The RKDS is maintained by
    // the drives and is retrieved from the drive when the RKDS is read.
    RKTypes::RKER rker_ {0};
    RKTypes::RKCS rkcs_ {0};
    u16 rkwc_ {0};
    u16 rkba_ {0};
    RKTypes::RKDA rkda_ {0};
    u16 rkdb_ {0};

    // RKCS writable bits mask. Bits 0-11 are read/write, except for bit 7
    // (Control Ready), and bits 12-15 are write protected. Bit 9 (Extra Bit)
    // is unused but is read/write.
    static const u16 rkcsWritableBits       {07577};

    // Definition of (pointers to) the attached RK05 drives
    vector<unique_ptr<RK05>> rk05Drives_ {};

    Bus* bus_ {nullptr};

public:
    // Constructor
    RK11D (Bus* bus, Window* window, shared_ptr<RK11DConfig> rk11dConfig);
    ~RK11D ();

    // Functions required by the BusDevice interface
    CondData<u16> read (BusAddress busAddress) override;
    StatusCode writeWord (BusAddress busAddress, u16 value) override;
    bool responsible (BusAddress busAddress) override;
    void reset () override;

    // Functions required by the DriveInterface interface
    void reportSeekComplete (RKTypes::SeekCompleteReport report);

private:
    // Definition of the controller's base address and vector
    u16 baseAddress_ {0};
    u16 vector_ {0};

    // Definition of the currently selected drive. The drive is selected by
    // either setting the Drive Select bits in the RKDA or by an interrupt
    // from a drive.
    u16 selectedDrive_ {0};

    // Action processor thread
    thread functionProcessorThread_;

    // Safe guard against controller access from multiple threads
    mutex controllerMutex_;

    // Condition variable to wake up the function processor when a function has
    // been queued.
    condition_variable functionAvailable_;

    binary_semaphore interruptRequestGranted_ {1};

    // Definition of the function processor states
    struct WaitingForFunction {};
    struct ProcessingFunction { RKTypes::Function function; };
    struct Polling { RKTypes::Function function; };

    using State = variant<WaitingForFunction,
        ProcessingFunction, Polling, monostate>;

    // Definition of the function processor events
    using Event = variant<RKTypes::Function,
        RKTypes::SeekCompleteReport>;

    // Use the PIMPL idiom to be able to define the StateMachine outside
    // of the RK05 class
    class StateMachine;
    unique_ptr<StateMachine> functionProcessorStateMachine_;

    // Definition of the queue for forwarding issued functions to the function
    // processor. The queue is accessed from multiple threads and its consistency
    // has to be safe-guarded by the controllerMutex_.
    ThreadSafeQueue<Event> functionQueue_;

    // Definition of a buffer for the data to be transferred to/from the
    // RK05 drive
    unique_ptr<u16[]> buffer_;

    // Definition of the RK11 function steps
    bool driveReady (RKTypes::Function function);
    RKTypes::CommandCompletion driveRead (RKTypes::Function function,
        RKTypes::CommandCompletion& commandCompletion);
    RKTypes::CommandCompletion driveReadHeader (RKTypes::Function function,
        RKTypes::CommandCompletion& commandCompletion);
    bool driveSeek (RKTypes::Function function,
        RKTypes::CommandCompletion& commandCompletion);
    void waitTillSeekCompleted ();

    void functionProcessor ();
    void processFunction (RKTypes::Function function);
    void executeSeek (RKTypes::RKDA diskAddress);
    void executeRead (RKTypes::Function function);
    void executeReadCheck (RKTypes::Function function);
    void executeWriteCheck (RKTypes::Function function);
    void executeWrite (RKTypes::Function function);
    void executeDriveReset (RKTypes::Function function);
    void executeWriteLock (RKTypes::Function function);
    StatusCode transferDataToBuffer (BusAddress memoryAddress,
        u16 wordCount, unique_ptr<u16[]>& buffer);
    StatusCode transferPatternToBuffer (BusAddress memoryAddress,
        u16 wordCount, unique_ptr<u16[]>& buffer);
    StatusCode transferDataFromBuffer (BusAddress memoryAddress,
        u16 wordCount, unique_ptr<u16[]>& buffer);
    StatusCode transferWordFromBuffer (BusAddress memoryAddress,
        u16 wordCount, unique_ptr<u16[]>& buffer);
    StatusCode compareDataWithBuffer (BusAddress memoryAddress,
        u16 wordCount, unique_ptr<u16[]>& buffer);
    StatusCode comparePatternWithBuffer (BusAddress memoryAddress,
        u16 wordCount, unique_ptr<u16[]>& buffer);
    void BINITReceiver (bool signalValue);
    void setNonExistingDisk ();
    void setControlReady ();
    void finish ();
    u32 absValueFromTwosComplement (u16 value) const;
    bool functionParametersOk (RKTypes::Function function);
    void setError (function<void ()> function);
    RKTypes::RKDS getDriveStatus (u16 driveId);
    void startFunction ();
    BusAddress busAddressFromRegs ();
    void busAddressToRegs (u32 busAddress);
};

// Definition of the state machine for the function processor. The class has
// to be defined in the same compilation unit to prevent incomplete type
// compilation errors.
class RK11D::StateMachine :
    public variantFsm::Fsm<StateMachine,
        Event, State>
{
public:
    StateMachine (RK11D* context);

    void entry (WaitingForFunction);
    State transition (WaitingForFunction&&,
        RKTypes::Function);                                 // -> ProcessingFunction
    State transition (WaitingForFunction&&,
        RKTypes::SeekCompleteReport);
    
    void entry (ProcessingFunction);
    State transition (ProcessingFunction&&,
        RKTypes::Function);                                 // -> ProcessingFunction/Polling
    State transition (ProcessingFunction&&,
        RKTypes::SeekCompleteReport);
    
    void entry (Polling);
    State transition (Polling&&,
        RKTypes::SeekCompleteReport);                       // -> Polling
    State transition (Polling&&,
        RKTypes::Function);                                 // -> Polling/ProcessingFunction

    // Define the default transition for transitions not explicitly
    // defined above. The default transition implies the event is ignored.
    template <typename S, typename E>
    State transition (S&& state, E)
    {
        return monostate {};
    }

    // As we make use of entry functions, we must handle all cases.
    // The default entry action is an immediate return.
    template <typename S> void entry (S&) {}

private:
    RK11D* context_;

    void completeSeek (RKTypes::SeekCompleteReport report);
};

#endif // _RK11D_H_