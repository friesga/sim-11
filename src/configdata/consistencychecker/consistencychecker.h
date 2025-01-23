#ifndef _CONSISTENCYCHECKER_H_
#define _CONSISTENCYCHECKER_H_

#include "../deviceconfig/deviceconfig.h"
#include "../systemconfig/systemconfig.h"

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
    ConsistencyChecker (SystemConfig const& systemConfig);
    void checkAll ();

    // The following consistency checkers are defined public to be able
    // to execute them from unit tests.
    void checkBA11Consistency ();
    void checkQbusConsistency ();
    void checkUnibusConsistency ();
    void checkConsoleConsistency ();
    template<typename TConfig, size_t capacity> void checkMS11Consistency ();
    void checkKDF11_BConsistency ();
    void checkCabinetConsistency ();
    void checkM9312Consistency ();

private:
    SystemConfig const& systemConfig_;

    void checkEitherBA11_NOrBA11_L ();
    void checkOneBA11 ();
    void checkBusConsistency ();
    void checkNotBothM9312AndKT24 ();
    void checkM9312OrKT24 ();

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