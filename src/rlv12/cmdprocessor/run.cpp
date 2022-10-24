#include "cmdprocessor.h"
#include "../rlv12command/rlv12command.h"

#include <mutex>
#include <condition_variable>

void CmdProcessor::run ()
{
    // Guard against controller register access from writeWord()
    std::unique_lock<std::mutex> lock {controller_->controllerMutex_};

    // Report the controller is ready to receive command
    controller_->csr_ |= RLV12::CSR_ControllerReady;

    while (running_)
    {
        // Wait to be signalled by writeWord() that a command has been
        // given in the CSR or the controller has to quit
        // ToDo: Take into account spurious wakeups
        controller_->signal_.wait(lock);

        if (!running_)
            break;

        // Get a  reference to the unit for which the command is destined.
        RL01_2 &unit = controller_->units_[RLV12::getDrive (controller_->csr_)];

        // Assemble a command from the command given in the CSR and the
        // parameters in the other registers
        // ToDo: Assemble command in the constructor?!
        RLV12Command rlv12Command (RLV12::getFunction (controller_->csr_), controller_->dar_,
            controller_->memAddrFromRegs (), 
            0200000 - controller_->wordCounter_);

        // Execute the command
        // ToDo: Merge the command pointer with the function pointer?
        u16 rlcsValue = (this->*commands_[rlv12Command.function_])
            (&unit, rlv12Command);

        // Set Controller Ready for the next command.
        controller_->setDone (unit, rlcsValue);
    }
}