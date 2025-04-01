#include "cmdprocessor.h"
#include "../rlv12command/rlv12command.h"

#include <mutex>
#include <condition_variable>
#include <iostream>

using std::cerr;

void CmdProcessor::run ()
try
{
    // Guard against controller register access from writeWord()
    std::unique_lock<std::mutex> lock {controller_->controllerMutex_};

    // Report the controller is ready to receive command
    controller_->csr_ |= RLV12const::CSR_ControllerReady;

    while (running_)
    {
        // Wait to be signalled by writeWord() that a command has been
        // given in the CSR or the controller has to quit
        controller_->signal_.wait (lock,
            [this] () {return !controller_->commandQueue_.empty () || !running_;});

        if (!running_)
            break;

        while (!controller_->commandQueue_.empty ())
        {
            RLV12Command rlv12Command =  controller_->commandQueue_.front ();

            // Get a  reference to the unit for which the command is destined.
            RL01_02& unit = controller_->units_[rlv12Command.unit_];

            // Execute the command
            u16 rlcsValue = (this->*commands_[rlv12Command.function_])
                (&unit, rlv12Command);

            // The event has been processed
            controller_->commandQueue_.pop ();

            // Set Controller Ready for the next command.
            controller_->setDone (unit, rlcsValue);
        }
    }
}
catch (const std::exception& ex)
{
    cerr << "CmdProcessor::run exception: " << ex.what () << '\n';
}
