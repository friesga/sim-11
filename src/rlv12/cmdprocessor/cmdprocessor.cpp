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
    finish ();
    cmdProcessorThread_.join();
}

// SignalX the command processor to stop processing
void CmdProcessor::finish ()
{
    // Guard against controller register access from writeWord()
    std::unique_lock<std::mutex> lock {controller_->controllerMutex_};

    running_ = false;

    // Wake up the command processor
    controller_->signal_.notify_one ();
}