#ifndef _CONSISTENCYCHECKER_H_
#define _CONSISTENCYCHECKER_H_

#include "../deviceconfig/deviceconfig.h"

#include <vector>

using std::vector;

// This class contains several configuration data consistency checkers.
// Its functions can be called either from the ini file processor
// or from the PDP-11 consistency check. These checks differ to allow
// unit tests to be performed on minimal configurations.
//
// The checks are designed as public functions to be able to execute them
// separately. The checkConsistency function will perform all checks.
class ConsistencyChecker
{
public:
    void checkConsistency (vector<DeviceConfig> const& systemConfig);
    void checkConsoleConsistency (vector<DeviceConfig> const& systemConfig);

private:

    template<typename T> static bool findDevice (DeviceConfig device);
};

#endif // _CONSISTENCYCHECKER_H_