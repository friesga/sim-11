#include "rk/rk11d/rk11d.h"
#include "ms11p/ms11p.h"
#include "bus/unibus/unibus.h"
#include "statuscodes.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <gtest/gtest.h>
#include <memory>
#include <chrono>

using std::make_shared;

using namespace std::chrono_literals;

// Definition of the test fixture
class RK11DWriteTest : public ::testing::Test
{
protected:
    // Use our own set of definitions to avoid common cause issues
    static constexpr u16 RK11D_BASE = 0177400;
    static constexpr u16 RKDS = RK11D_BASE;
    static constexpr u16 RKER = RK11D_BASE + 02;
    static constexpr u16 RKCS = RK11D_BASE + 04;
    static constexpr u16 RKWC = RK11D_BASE + 06;
    static constexpr u16 RKBA = RK11D_BASE + 010;
    static constexpr u16 RKDA = RK11D_BASE + 012;
    static constexpr u16 RKDB = RK11D_BASE + 016;

    // RKDS bit definitions
    static constexpr u16  RKDS_DRY = (1 << 7);
    inline u16 getRKDSdriveId (u16 rkds) { return (rkds & 7) >> 13; }

    // RKER bit definitions
    static constexpr u16  RKER_NXS = (1 << 5);
    static constexpr u16  RKER_NXC = (1 << 6);
    static constexpr u16  RKER_NXD = (1 << 7);
    static constexpr u16  RKER_DRE = (1 << 15);

    // RKCS bit definitions
    static constexpr u16  RKCS_GO  = (1 << 0);
    static constexpr u16  RKCS_RDY = (1 << 7);
    static constexpr u16  RKCS_HE  = (1 << 14);
    static constexpr u16  RKCS_ERR = (1 << 15);
    inline u16 RKCS_OPERATION (u16 function) { return (function & 7) << 1; }


    // Function definitions
    enum Operation
    {
        ControlReset = 0,
        Write,
        Read,
        WriteCheck,
        Seek,
        ReadCheck,
        DriveReset,
        WriteLock
    };

    void waitForControllerReady (RK11D* controller)
    {
        u16 result;
        do
        {
            SimulatorClock::forwardClock (10ms);
            result = controller->read (RKCS);
        } while (!(result & RKCS_RDY));
    }

    void waitForDriveReady (RK11D* controller, u16 driveId)
    {
        u16 result;
        do
        {
            SimulatorClock::forwardClock (10ms);
            result = controller->read (RKDS);
        } while (!((result & RKDS_DRY) && getRKDSdriveId (result) == driveId));

    }
};

// Verify an drive error is reported for a non-ready drive. The specification
// of a spin up time in the configuration causes the drive not be spun up.
TEST_F (RK11DWriteTest, writeOnNonReadyDriveReportsError)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] =
        make_shared<RK05Config> (RK05Config
        ({
            .fileName = "rk05.dsk",
            .newFile = true,
            .overwrite = true,
            .spinUpTime = 100
            }));

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr,
        make_shared<RK11DConfig> (rk11dConfig));

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Write) | RKCS_GO), StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}) & RKER_DRE, RKER_DRE);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDS}) & RKDS_DRY, 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}) & (RKCS_ERR | RKCS_HE),
        RKCS_ERR | RKCS_HE);
}
