#include "rk05.h"

#include <functional>

using std::bind;

// ToDo: Uniform parameters with writeDataToSector()
void RK05::write (DiskAddress diskAddress, u16 wordCount, u16* data)
{
    sendTrigger (SeekCommand {seekTime (currentCylinderAddress_, diskAddress.cylinder),
        [&] { size_t wordsWritten = diskDrive_.writeDataToSector (diskAddress, data, wordCount);
                controller_->dataTransferComplete (wordsWritten); }});
}

void RK05::read (DiskAddress diskAddress, u16 wordCount, u16* data)
{
    sendTrigger (SeekCommand {seekTime (currentCylinderAddress_, diskAddress.cylinder),
        [&] { size_t wordsRead =
                diskDrive_.readDataFromSector (diskAddress, data, wordCount);
              controller_->dataTransferComplete (wordsRead); }});
}