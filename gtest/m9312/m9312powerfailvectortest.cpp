#include "pdp11/pdp11.h"
#include "m9312/m9312.h"
#include "configdata/m9312/m9312config/m9312config.h"

#include <gtest/gtest.h>
#include <memory>

using std::shared_ptr;
using std::make_shared;

// Verify the M9312 is responsible for the powerfail vector when battery
// backup is not available and the first read's after power-up are for
// the powerfail vector. On a second read of these addresses the m9312
// is no longer responsible.
TEST (M9312PowerfailVectorTest, noBatteryBackup)
{
    M9312Config config =
    {
        M9312Config::DiagROMType::_23_248F1,
        {M9312Config::BootROMType::_23_751A9, M9312Config::BootROMType::_23_752A9,
        M9312Config::BootROMType::_23_753A9, M9312Config::BootROMType::_23_755A9},
        0,
        true
    };

    shared_ptr<M9312Config> m9312ConfigPtr = make_shared<M9312Config> (config);

    Qbus bus;
    M9312 m9312 (&bus, m9312ConfigPtr);

    bus.BatteryPower ().set (false);

    EXPECT_TRUE (m9312.responsible (024));
    EXPECT_TRUE (m9312.responsible (026));
}

// Verify the M9312 is not responsible for the powerfail vector when battery
// backup is available
TEST (M9312PowerfailVectorTest, batteryBackup)
{
    M9312Config config =
    {
        M9312Config::DiagROMType::_23_248F1,
        {M9312Config::BootROMType::_23_751A9, M9312Config::BootROMType::_23_752A9,
        M9312Config::BootROMType::_23_753A9, M9312Config::BootROMType::_23_755A9},
        0,
        true
    };

    shared_ptr<M9312Config> m9312ConfigPtr = make_shared<M9312Config> (config);

    Qbus bus;
    M9312 m9312 (&bus, m9312ConfigPtr);

    bus.BatteryPower ().set (true);

    EXPECT_FALSE (m9312.responsible (024));
    EXPECT_FALSE (m9312.responsible (026));
}


// Verify the M9312 is not responsible for the powerfail vector when the
// first read after power-up is to another address than the powerfail vector.
TEST (M9312PowerfailVectorTest, notFirstRead)
{
    M9312Config config =
    {
        M9312Config::DiagROMType::_23_248F1,
        {M9312Config::BootROMType::_23_751A9, M9312Config::BootROMType::_23_752A9,
        M9312Config::BootROMType::_23_753A9, M9312Config::BootROMType::_23_755A9},
        0,
        true
    };

    shared_ptr<M9312Config> m9312ConfigPtr = make_shared<M9312Config> (config);

    Qbus bus;
    M9312 m9312 (&bus, m9312ConfigPtr);

    bus.BatteryPower ().set (false);

    EXPECT_FALSE (m9312.responsible (0));
    EXPECT_FALSE (m9312.responsible (024));
    EXPECT_FALSE (m9312.responsible (026));
}

// Verify the M9312 is responsible for the powerfail vector after a power-up
TEST (M9312PowerfailVectorTest, afterPowerUp)
{
    M9312Config config =
    {
        M9312Config::DiagROMType::_23_248F1,
        {M9312Config::BootROMType::_23_751A9, M9312Config::BootROMType::_23_752A9,
        M9312Config::BootROMType::_23_753A9, M9312Config::BootROMType::_23_755A9},
        0,
        true
    };

    shared_ptr<M9312Config> m9312ConfigPtr = make_shared<M9312Config> (config);

    Qbus bus;
    M9312 m9312 (&bus, m9312ConfigPtr);

    bus.BatteryPower ().set (false);

    EXPECT_FALSE (m9312.responsible (0));

    // After a read from another address the M9312 is not responsible for
    // the powerfail vector.
    EXPECT_FALSE (m9312.responsible (024));
    EXPECT_FALSE (m9312.responsible (026));

    // Make sure that power is applied and the M9312 is notified
    EXPECT_FALSE (bus.BPOK ());
    bus.BPOK ().set (true);

    EXPECT_TRUE (m9312.responsible (024));
    EXPECT_TRUE (m9312.responsible (026));
}

// Verify the M9312 is not responsible for the powerfail vector when the 
// power-up-boot-enable option is set false
TEST (M9312PowerfailVectorTest, powerUpBootEnableFalse)
{
    M9312Config config =
    {
        M9312Config::DiagROMType::_23_248F1,
        {M9312Config::BootROMType::_23_751A9, M9312Config::BootROMType::_23_752A9,
        M9312Config::BootROMType::_23_753A9, M9312Config::BootROMType::_23_755A9},
        0,
        false
    };

    shared_ptr<M9312Config> m9312ConfigPtr = make_shared<M9312Config> (config);

    Qbus bus;
    M9312 m9312 (&bus, m9312ConfigPtr);

    bus.BatteryPower ().set (true);

    EXPECT_FALSE (m9312.responsible (024));
    EXPECT_FALSE (m9312.responsible (026));
}