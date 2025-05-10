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
        {
            DiskAddress diskAddress
            {
                rkCommand.diskAddress.sectorAddress,
                rkCommand.diskAddress.surface,
                rkCommand.diskAddress.cylinderAddress
            };
            size_t wordCount = context_->diskDrive_.writeDataToSector (diskAddress,
                context_->buffer_.get (), rkCommand.wordCount);

            RKDefinitions::RKDS rkds {0};
            rkds.sectorCounter = rkCommand.diskAddress.sectorAddress +
                wordCount / context_->rk05Geometry.wordsPerSector ();
            rkds.sectorCounterEqualsSectorAddress = 1;
            rkds.readWriteSeekReady = 1;
            rkds.driveReady = 1;
            rkds.sectorCounter = 1;
            rkds.rk05DiskOnLine = 1;
            // ToDo: Set driveId

            // It is safe to cast the size_t wordCount to an u16 as the
            // maximum word count is 2 ^16.
            context_->controller_->processResult (RKDefinitions::Result {
                rkds, 0, static_cast<u16> (wordCount), 0});

            // ToDo: Transfer has to be done via RKDB?!
            break;
        }

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