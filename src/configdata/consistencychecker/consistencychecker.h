#ifndef _CONSISTENCYCHECKER_H_
#define _CONSISTENCYCHECKER_H_

#include "../deviceconfig/deviceconfig.h"

#include <vector>

using std::vector;

// This class contains several configuration data consistency
// checks.  The checks are designed as public functions to be able
// to execute them separately. The checkAll function is meant to be executed
// at the start of the simulator. This allows the unit tests to define
// minimal configurations not meeting all requirements.
class ConsistencyChecker
{
public:
    ConsistencyChecker (vector<DeviceConfig> const& systemConfig);
    void checkAll ();
    void checkConsoleConsistency ();
    void checkMSV11Consistency ();
    void checkKDF11_BConsistency ();
    void checkCabinetConsistency ();

private:
    vector<DeviceConfig> const& systemConfig_;

    template<typename T> static bool findDevice (DeviceConfig device);
    bool conflictsWith (shared_ptr<MSV11Config> msv11Card1, 
		shared_ptr<MSV11Config> msv11Card2);
	bool isWithin (u32 address, u32 begin, u32 end);
};

#endif // _CONSISTENCYCHECKER_H_