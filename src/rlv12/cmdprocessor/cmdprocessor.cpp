#include "cmdprocessor.h"

// The constructor starts the command processor in a separate thread
CmdProcessor::CmdProcessor (RLV12 *controller)
    :
    cmdProcessorThread_ {std::thread(&CmdProcessor::run, this)},
	running_ {true},
    controller_ {controller}
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
    return (getTrack (diskAddress) * RL_NUMSC +
        getSector(diskAddress)) * RL_NUMWD * sizeof(int16_t);
}