#include "rk05.h"

#include <functional>

using std::bind;

// This function object is executed on the completion of a seek which is part
// of a write operation.
//
class RK05::WriteCompletion
{
public:
    WriteCompletion (DiskDrive diskDrive, DiskAddress diskAddress, u16 wordCount, u16* data)
        :
        diskDrive_ {diskDrive},
        diskAddress_ {diskAddress},
        wordCount_ {wordCount},
        data_ {data}
    {}

    size_t operator () ()
    {
        return diskDrive_.writeDataToSector (diskAddress_, data_, wordCount_);
    }

private:
    DiskDrive diskDrive_;
    DiskAddress diskAddress_;
    u16 wordCount_;
    u16* data_;
};

// ToDo: Uniform parameters with writeDataToSector()
void RK05::write (DiskAddress diskAddress, u16 wordCount, u16* data)
{
    // Create a function object with the required parameters for the 
    // writeDataToSector call. This function object will be called after
    // completion of the seek command.
    // WriteCompletion writeCompletion {diskDrive_, diskAddress, wordCount, data};

    size_t bytesWritten {0};

    // sendTrigger (SeekCommand {seekTime (currentCylinderAddress_, diskAddress.cylinder),
    //    [this, &writeCompletion, &bytesWritten] { bytesWritten = writeCompletion (); }});

    sendTrigger (SeekCommand {seekTime (currentCylinderAddress_, diskAddress.cylinder),
        [&] { bytesWritten = diskDrive_.writeDataToSector (diskAddress, data, wordCount);
                controller_->dataTransferComplete (bytesWritten); }});
}