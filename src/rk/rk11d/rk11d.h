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
using std::bind;
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

    // The function processor states and events are defined public to be
    // able to use them in the trace functions.
    struct WaitingForFunction {};
    struct ProcessingFunction { RKTypes::Function function; };
    struct Polling { RKTypes::Function function; };

    using State = variant<WaitingForFunction,
        ProcessingFunction, Polling, monostate>;

    // Definition of the function processor events
    using Event = variant<RKTypes::Function,
        RKTypes::SeekCompleteReport>;

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

    // Use the PIMPL idiom to be able to define the StateMachine outside
    // of the RK05 class
    class StateMachine;
    unique_ptr<StateMachine> stateMachine_;

    // Definition of the queue for forwarding issued functions to the function
    // processor. The queue is accessed from multiple threads and its consistency
    // has to be safe-guarded by the controllerMutex_.
    ThreadSafeQueue<Event> functionQueue_;

    // Definition of a buffer for the data to be transferred to/from the
    // RK05 drive
    unique_ptr<u16[]> buffer_;

    enum class SeekMode {Sync, Async};

    // The RK11 functions are executed in a sequence of steps. Every RK11
    // function has its own sequence. Every step is a C++ function, returning
    // true if the function is executed succesfully and the step sequence can
    // be continued and false if not, indicating the RK11 function has to be
    // aborted.
    // 
    // Every step function has one or two parameters, the RK11 function and
    // the FunctionResult struct which gets the result of the execution of
    // the function.
    //
    bool driveReady (RKTypes::Function const & function);
    bool functionParametersOk (RKTypes::Function const& function);
    bool notWriteProtected (RKTypes::Function const& function);
    bool cylinderAddressOk (RKTypes::Function const& function);
    void driveRead (RKTypes::Function const& function,
        RKTypes::FunctionResult& functionResult);
    void driveReadHeader (RKTypes::Function const& function,
        RKTypes::FunctionResult& functionResult);
    void driveWrite (RKTypes::Function const& function,
        RKTypes::FunctionResult&);
    bool seek (RKTypes::Function const& function, SeekMode mode);
    void waitTillSeekCompleted (u16 driveId);
    bool updateRegisters (RKTypes::Function const& function,
        RKTypes::FunctionResult& functionResult);
    bool writeBufferToMemory (RKTypes::Function const& function,
        RKTypes::FunctionResult& functionResult);
    bool readBufferFromMemory (RKTypes::Function const& function,
        RKTypes::FunctionResult& functionResult);
    bool compareBufferWithMemory (RKTypes::Function const& function,
        RKTypes::FunctionResult& functionResult);
    void setWritCheckOnError (RKTypes::FunctionResult& functionResult);
    void driveWriteLock (RKTypes::Function const& function);
    void clearBufferToEndOfSector (RKTypes::Function const& function);

    using Step = function<bool (RKTypes::Function,
        RKTypes::FunctionResult&)>;
    using StepVector = vector<Step>;

    // Disclaimer: The syntax is extremely ugly but using lambdas instead
    // of function pointers allows more flexibility in the calling sequence.
    // 
    // Ideally the definition of the step vectors would be static. In that
    // case howver we would lose the reference to the current object via
    // the this pointer.
    StepVector const writeFunction_ =
    {
        [this] (RKTypes::Function const & function, RKTypes::FunctionResult& functionResult)
            { return driveReady (function); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return notWriteProtected (function); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return functionParametersOk (function); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return readBufferFromMemory (function, functionResult); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { clearBufferToEndOfSector (function); return true; },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return seek (function, SeekMode::Sync); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { driveWrite (function, functionResult); return true; },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return updateRegisters (function, functionResult); }
    };

    StepVector const readFunction_ =
    {
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return driveReady (function); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return functionParametersOk (function); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return seek (function, SeekMode::Sync); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { driveRead (function, functionResult); return true; },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { writeBufferToMemory (function, functionResult); return true; },
        // ToDo: Clear the part of the buffer not filled by the read
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return updateRegisters (function, functionResult); }
    };

    StepVector const readHeaderFunction_ =
    {
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return driveReady (function); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return functionParametersOk (function); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return seek (function, SeekMode::Sync); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { driveReadHeader (function, functionResult); return true; },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { writeBufferToMemory (function, functionResult); return true; },
        // ToDo: Clear the part of the buffer not filled by the read
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return updateRegisters (function, functionResult); }
    };

    // The Write Check function is used to compare the contents of memory to the
    // contents of a continuous block of data on a disk cartridge. The controller
    // first performs a Seek function, just as for a Write function, and then
    // reads and checks the next header word. If the cylinder address is correct,
    // the controller waits for SC = SA, then begins reading the rest of the
    // sector (Data and Checksum) while performing BUS NPR transfers for each data
    // word. Each data word from the disk drive is compared, bit by bit, with
    // memory data from the Unibus. The disk drive checksum, in turn, is compared
    // with the checksum calculated by the controller. If any bit is found to be
    // in error, RKER 00 (Write Check Error) is set. Controller reaction is then
    // determined by RKCS 06 (IDE) and RKCS 08 (SSE). The Write Check function may
    // be performed on a short sector (less than 256 data words) as long as the
    // number of words write checked is equal to the number of words previously
    // written into the sector. (EK-RK11D-MM-002 par. 1.3.2.7)
    //
    // ToDo: Value of RKBA and RKWC probably has to be adapted to the result of
    // the data comparison.
    //
    // In this sequence the result of compareBufferWithMemory() is ignored
    // as that would terminate the sequence. On an error the functionResult
    // status code is set which in the last step will set the write check
    // error.
    StepVector const writeCheckFunction_ =
    {
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return driveReady (function); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return functionParametersOk (function); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return seek (function, SeekMode::Sync); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { driveRead (function, functionResult); return true; },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { compareBufferWithMemory (function, functionResult); return true; },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return updateRegisters (function, functionResult); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { setWritCheckOnError (functionResult); return true; }
    };

    // The Read Check function is identical to a normal Read function, except
    // that no NPRs occur. Only the checksum is calculated and compared with the
    // checksum read from the disk drive. This function enables the program to
    // know beforehand if a given block of data is readable and error free.
    // Because the Read Check is essentially a parity check, it must be performed
    // on a whole-sector basis only. (EK-RK11D-MM-002)
    // 
    // As the parity check is not implemented this function essentially is a NOP
    // and just checks the parameters.
    //
    StepVector const readCheckFunction_ =
    {
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return driveReady (function); },
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return functionParametersOk (function); },
    };

    StepVector const controlResetFunction_ =
    {
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { reset (); return true; },
    };

    // For a Seek function, the RK11 directs the selected disk drive to move its
    // head mechanism to the cylinder address specified by RKDA 05 through 12.
    // When this portion of a Seek has been initiated, the controller returns to
    // the Ready state (RKCS 07). But if the specified cylinder address is greater
    // than 0312, the function is aborted and bit 06 (nonexistent Cylinder) of the
    // RKER is set. RKCS 06 (Interrupt Done Enable) then determines the program
    // reaction. (EK-RK11D-MM-002 par. 1.3.2.2)
    //
    // The acceptance (Address Acknowledge) of a Seek or Srive Reset function by
    // the selected drive generates an interrupt request.
    // (EK-RK11D-MM-002, par. 3.4)
    // 
    // Presumably the interrupt is only generated when RKCS IDE is set.
    //
    StepVector const seekFunction_ =
    {
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return seek (function, SeekMode::Async); },
    };

    // For a Drive Reset function, the controller directs the selected disk drive
    // to move its head mechanism to cylinder address 000 and reset all active
    // error status lines. To the controller, the Drive Reset function is the same
    // as a Seek function, even to the manner in which the hardware poll logic is
    // used; however, a Drive Reset function can take much longer than a Seek
    // function to execute. The time required to complete a Drive Reset function
    // depends on the physical position of the head mechanism at the time the
    // function is initiated, and therefore may take up to a maximum of two
    // seconds. (EK-RK11D-MM-002)
    //
    StepVector const driveResetFunction_ =
    {
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { return seek (function, SeekMode::Async); },
    };

    // The Write Lock function write-protects a selected disk drive until the
    // condition is overridden by operation of the corresponding WT PROT
    // (Write Protect) switch on the disk drive (refer to RK05 Disk Drive
    // Maintenance Manual, DEC-00-HRKO05-C-D). The disk drive is automatically
    // write-enabled when power is first applied, or when the disk drive RUN/LOAD
    // switch is set to RUN.
    // (EK-RK11D-MM-002, par. 1.3.2.4)
    //
    StepVector const writeLockFunction_ =
    {
        [this] (RKTypes::Function const& function, RKTypes::FunctionResult& functionResult)
            { driveWriteLock (function); return true; },
    };

    // The step vectors have to be ordered in Operation sequence as the 
    // Operation is used as in index into the vector by the function
    // processor.
    vector<StepVector> const rk11dFunctions =
    {
        controlResetFunction_,
        writeFunction_,
        readFunction_,
        writeCheckFunction_,
        seekFunction_,
        readCheckFunction_,
        driveResetFunction_,
        writeLockFunction_
    };

    void functionProcessor ();
    void processFunction (RKTypes::Function const& function);

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
    u16 absValueFromTwosComplement (u16 value) const;
    void setError (function<void ()> function);
    RKTypes::RKDS getDriveStatus (u16 driveId);
    void startFunction ();
    BusAddress busAddressFromRegs ();
    void busAddressToRegs (u32 busAddress);
    void setSeekError ();
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
        RKTypes::Function const&);                                 // -> ProcessingFunction
    State transition (WaitingForFunction&&,
        RKTypes::SeekCompleteReport);
    
    void entry (ProcessingFunction);
    State transition (ProcessingFunction&&,
        RKTypes::Function const&);                                 // -> ProcessingFunction/Polling
    State transition (ProcessingFunction&&,
        RKTypes::SeekCompleteReport);
    
    void entry (Polling);
    State transition (Polling&&,
        RKTypes::SeekCompleteReport);                       // -> Polling
    State transition (Polling&&,
        RKTypes::Function const&);                                 // -> Polling/ProcessingFunction

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