#ifndef _RLV12MAINTENANCECMD_H_
#define _RLV12MAINTENANCECMD_H_

#include "rlv12command.h"

class RLV12MaintenanceCmd : public RLV12Command
{
public:
    // Use base class constructor
    using RLV12Command::RLV12Command;

    void execute (RLV12 *controller, RL01_2 *unit) override;
    void finish (RLV12 *controller, RL01_2 *unit) override;
};

#endif // !_RLV12MAINTENANCECMD_H_
