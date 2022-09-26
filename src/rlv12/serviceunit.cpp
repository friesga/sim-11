#include "rlv12.h"

#include <mutex>

using std::mutex;
using std::lock_guard;

//
// This function services a RLV12Command for a specific unit. Every unit
// has its own service function, running in a separate thread. This allows
// to execute Seek commands for several units running in parallel.
// 
// The function is excuted in the context of the controller, as it needs to
// access the controllers registers. Simultaneous access to the registers is
// safeguarded by a mutex.
//
void RLV12::service (Unit& unitRef,
    ThreadSafeQueue<RLV12Command *> &commandQueue)
{
    RL01_2& unit = static_cast<RL01_2&> (unitRef);
    RLV12Command  *rlv12Command;

    // EK-0RL11-TD-001, p2-3: "If the CPU software initiates another
    // operation on a drive that is busy seeking, the controller will
    // suspend the operation until the seek is completed."
    // This is implemented by queueinq commands to the service function
    // for the unit.
    // 
    // Get commands from the queue until the queue is closed
    while (commandQueue.waitAndPop (rlv12Command))
    {
        controllerMutex_.lock();
        rlcs &= ~RLCS_DRDY;
        controllerMutex_.unlock();

        // All commands require an attached unit, except the Maintenance and
        // possibly the Get Status commands.
        // ToDo: Move this test to RLV12Commands
#if 0
        if ((unit.unitStatus_ & Status::UNIT_DIS ||
            unit.rlStatus_ & RlStatus::UNIT_OFFL ||
            !(unit.unitStatus_ & Status::UNIT_ATT)) &&
            (unit.function_ != RLCS_MAINTENANCE || unit.function_ != RLCS_GSTA))
        {
            TRACERLV12Registers (&trc, "Command incomplete",
                rlcs, rlba, rlda, rlmpr, rlbae);

            // Spin error
            unit.driveStatus_ |= RLDS_SPE;

            // Flag error
            setDone (RLCS_ERR | RLCS_INCMP);
            return;
        }
#endif

        // All commands are processed in two steps:
        // 1. Execution of the command,
        // 2. Finishing the execution.
        //
        // Note that not all commands require both steps.

        rlv12Command->execute (this, &unit);
        rlv12Command->finish (this, &unit);
    }
}