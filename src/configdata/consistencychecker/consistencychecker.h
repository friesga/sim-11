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

    void checkBA11Consistency ();
    void checkConsoleConsistency ();
    template<typename TConfig, size_t capacity> void checkMS11Consistency ();
    void checkKDF11_BConsistency ();
    void checkCabinetConsistency ();

private:
    vector<DeviceConfig> const& systemConfig_;

    bool isQbusSystem ();
    void checkQbusConfiguration ();
    void checkUnibusConfiguration ();
    void checkEitherBA11_NOrBA11_L ();
    void checkOneBA11 ();
    void checkBusConsistency ();

    template<typename T> static bool findDevice (DeviceConfig device);
    template<typename TConfig> bool conflictsWith (shared_ptr<TConfig> msv11Card1,
		shared_ptr<TConfig> msv11Card2, size_t capacity);
	bool isWithin (u32 address, u32 begin, u32 end);
};

template<typename T>
bool ConsistencyChecker::findDevice (DeviceConfig device)
{
    return var_type (device) == typeid (shared_ptr<T> {});
}

#endif // _CONSISTENCYCHECKER_H_