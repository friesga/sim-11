#include "rk05.h"

#include <stdexcept>

using std::logic_error;

void RK05::StateMachine::handleFunction (RKDefinitions::RKCommand rkCommand)
{
    // Note that the ControlReset function is handled in the RK11-D and isn't
    // sent to the RK05.
    switch (rkCommand.function)
    {
        case RKDefinitions::Write:
        case RKDefinitions::Read:
        case RKDefinitions::WriteCheck:
        case RKDefinitions::Seek:
        case RKDefinitions::ReadCheck:
        case RKDefinitions::DriveReset:
        case RKDefinitions::WriteLock:

        default:
            throw logic_error ("Invalid function in RK05::StateMachine::handleFunction");
    }

    RKDefinitions::RKER rker {};
    rker.driveError = 1;

    context_->controller_->processResult (RKDefinitions::Result {
        0, rker, 0, 0});
}