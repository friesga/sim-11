#include "cmdprocessor.h"

// The constructor starts the command processor in a separate thread
CmdProcessor::CmdProcessor (RLV12 *controller)
    :
    running_ {true},
    controller_ {controller},
    cmdProcessorThread_ {std::thread(&CmdProcessor::run, this)}

{}

// The destructor indicates the running command processor to stop and
// waits for its completion
CmdProcessor::~CmdProcessor()
{
    running_ = false;

    // Wake up the command processor
    controller_->signal.notify_one ();

	cmdProcessorThread_.join();
}

// Calculate the position of a sector as an offset in the file from
// the specified diskAddress
int32_t CmdProcessor::filePosition (int32_t diskAddress) const
{
    return (RLV12::getTrack (diskAddress) * RLV12::sectorsPerSurface +
        RLV12::getSector(diskAddress)) * RLV12::wordsPerSector * 
        sizeof(int16_t);
}