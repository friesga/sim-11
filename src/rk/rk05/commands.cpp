#include "rk05.h"

#include <functional>

using std::bind;

// ToDo: Uniform parameters with writeDataToSector()
void RK05::write (DiskAddress diskAddress, u16 wordCount, u16* data)
{
    size_t bytesWritten {0};

    sendTrigger (SeekCommand {seekTime (currentCylinderAddress_, diskAddress.cylinder),
        [&] { bytesWritten = diskDrive_.writeDataToSector (diskAddress, data, wordCount);
                controller_->dataTransferComplete (bytesWritten); }});
}

void RK05::read (DiskAddress diskAddress, u16 wordCount, u16* data)
{
    size_t bytesRead {0};

    sendTrigger (SeekCommand {seekTime (currentCylinderAddress_, diskAddress.cylinder),
        [&] { bytesRead = diskDrive_.readDataFromSector (diskAddress, data, wordCount);
                controller_->dataTransferComplete (bytesRead); }});
}