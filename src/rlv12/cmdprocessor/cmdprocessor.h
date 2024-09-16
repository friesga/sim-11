#ifndef _CMDPROCESSOR_H_
#define _CMDPROCESSOR_H_

#include "../rlv12.h"
#include "../rlv12command/rlv12command.h"
#include "chrono/alarmclock/alarmclock.h"

#include <thread>
#include <vector>
#include <functional>

class RLV12;

// This function processes RLV12 commands. It forms the core of the
// RLV12 controller. The function is executed in a separate thread.
// This allows the processor to wait till a drive for which a command
// is issued to become ready and to implement the timing characterics
// of the specific commands.
class CmdProcessor
{
    bool running_;
    RLV12 *controller_;
    std::thread cmdProcessorThread_;
    AlarmClock alarmClock_;

    u16 maintenanceCmd (RL01_02 *unit, RLV12Command &rlv12Command);
    u16 writeCheckCmd (RL01_02 *unit, RLV12Command &rlv12Command);
    u16 getStatusCmd (RL01_02 *unit, RLV12Command &rlv12Command);
    u16 seekCmd (RL01_02 *unit, RLV12Command &rlv12Command);
    u16 readHeaderCmd (RL01_02 *unit, RLV12Command &rlv12Command);
    u16 writeDataCmd (RL01_02 *unit, RLV12Command &rlv12Command);
    u16 readDataCmd (RL01_02 *unit, RLV12Command &rlv12Command);
    u16 readDataWithoutHeaderCheckCmd (RL01_02 *unit, 
        RLV12Command &rlv12Command);

    // The pointers in the following vector must be in order of their numeric
    // value cf e.g. EK-RLV12-TD-001 page 3-17.
    typedef u16 (CmdProcessor::*fp) (RL01_02*, RLV12Command &rlv12Command);
    std::vector<fp> commands_
    {
        &CmdProcessor::maintenanceCmd,
        &CmdProcessor::writeCheckCmd,
        &CmdProcessor::getStatusCmd,
        &CmdProcessor::seekCmd,
        &CmdProcessor::readHeaderCmd,
        &CmdProcessor::writeDataCmd,
        &CmdProcessor::readDataCmd,
        &CmdProcessor::readDataWithoutHeaderCheckCmd
    };

    int32_t filePosition (int32_t diskAddress) const;

    // Definition for the procedures to calculate the new head position
    enum class HeadPositionProcedure
    {
        Increment,
        Rotate,
        DiskAddressRegister
    };

    u16 finishDataTransferCmd (RL01_02 *unit, RLV12Command &rlv12Command);
    void updateHeadPosition (CmdProcessor::HeadPositionProcedure procedure,
        RL01_02 *unit, int32_t wordCount);
    bool diskAddressOk (RL01_02 *unit, RLV12Command &rlv12Command);
    void limitWordCount (RLV12Command &rlv12Command);

public:
    // Constructor and destructor
    CmdProcessor (RLV12 *controller);
    ~CmdProcessor ();

    // The function running in the separate thread
    void run ();
    void finish ();
};

#endif // !_CMDPROCESSOR_H_