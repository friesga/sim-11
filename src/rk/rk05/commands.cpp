#include "rk05.h"

#include <functional>

using std::bind;

using namespace RKTypes;

// ToDo: Uniform parameters with writeDataToSector()
void RK05::write (DiskAddress diskAddress, u16 wordCount, u16* data)
{
    sendTrigger (SeekCommand {seekTime (currentCylinderAddress_, diskAddress.cylinder),
        [&] { size_t wordsWritten = writeDataToDrive (diskAddress, data, wordCount);
                controller_->dataTransferComplete (wordsWritten); }});
}

size_t RK05::writeDataToDrive (DiskAddress diskAddress, u16* buffer, u32 numWords)
{
    wtIndicator_->show (Indicator::State::On);
    size_t wordsWritten = diskDrive_.writeDataToSector (diskAddress, buffer,
        numWords);
    wtIndicator_->show (Indicator::State::Off);
    return wordsWritten;
}

void RK05::read (DiskAddress diskAddress, u16 wordCount, u16* data)
{
    sendTrigger (SeekCommand {seekTime (currentCylinderAddress_, diskAddress.cylinder),
        [&] { size_t wordsRead = diskDrive_.readDataFromSector (diskAddress, data, wordCount);
              controller_->dataTransferComplete (wordsRead); }});
}

size_t RK05::readDataFromDrive (DiskAddress diskAddress, u16* buffer,
    u32 wordCount)
{
    rdIndicator_->show (Indicator::State::On);
    size_t wordsRead = diskDrive_.readDataFromSector (diskAddress, buffer,
        wordCount);
    rdIndicator_->show (Indicator::State::Off);
    return wordsRead;
}

void RK05::seek (u16 cylinderAddress)
{
    sendTrigger (SeekCommand {seekTime (currentCylinderAddress_, cylinderAddress),
        [&] {controller_->reportSeekComplete (SeekCompleteReport {driveId_,
            driveError_}); }});

    // The current cylinder address actually should be set only when the
    // seek is completed, but as the seek cannot fail and the new cylinder
    // isn't available in the state machine transition we'll set it now.
    currentCylinderAddress_ = cylinderAddress;
}
