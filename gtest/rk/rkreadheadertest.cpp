#include "rk/rk11d/rk11d.h"
#include "mem/ms11p/ms11p.h"
#include "bus/unibus/unibus.h"
#include "statuscodes.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <gtest/gtest.h>
#include <chrono>

using namespace std::chrono_literals;

// Definition of the test fixture
// Definition of the test fixture
class RK11DReadHeaderTest : public ::testing::Test
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
    static constexpr u16  RKDS_SC_SA     = (1 << 4);
    static constexpr u16  RKDS_RWS_READY = (1 << 6);
    static constexpr u16  RKDS_DRY       = (1 << 7);
    static constexpr u16  RKDS_SOK       = (1 << 8);
    static constexpr u16  RKDS_RK05      = (1 << 11);
    constexpr u16 getRKDSdriveId (u16 rkds) { return (rkds >> 13); }

    // RKER bit definitions
    static constexpr u16  RKER_NXD = (1 << 7);

    // RKCS bit definitions
    static constexpr u16  RKCS_GO  = (1 << 0);
    static constexpr u16  RKCS_RDY = (1 << 7);
    static constexpr u16  RKCS_IDE = (1 << 6);
    static constexpr u16  RKCS_FMT = (1 << 10);
    static constexpr u16  RKCS_HE  = (1 << 14);
    static constexpr u16  RKCS_ERR = (1 << 15);
    inline u16 RKCS_OPERATION (u16 function) { return (function & 7) << 1; }

    // RKER bit definitions
    static constexpr u16  RKER_NXC = (1 << 6);

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

    RK11DConfig rk11dConfig {};
    Unibus bus;
    MS11P ms11p {&bus};
    RK11D* rk11dDevice;

    void SetUp () override
    {
        RK11DConfig rk11dConfig {};
        rk11dConfig.rk05Config[0] =
            RK05Config (RK05Config
            ({
                .unitNumber = 0,
                .fileName = "rk05.dsk",
                .newFile = true,
                .overwrite = true
                }));

        rk11dConfig.rk05Config[1] =
            RK05Config (RK05Config
            ({
                .unitNumber = 1,
                .fileName = "rk05-1.dsk",
                .newFile = true,
                .overwrite = true
                }));

        rk11dConfig.numUnits = 2;

        rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

        // Create a minimal system, consisting of just the bus, memory
        // and the RK11-D/RK05 to be tested.
        bus.installModule (&ms11p);
        bus.installModule (rk11dDevice);
    }

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

    void waitForRWSReady (RK11D* controller, u16 driveId)
    {
        u16 result;
        do
        {
            SimulatorClock::forwardClock (10ms);
            result = controller->read (RKDS);
        } while (!((result & RKDS_RWS_READY) &&
            getRKDSdriveId (result) == driveId));

    }

    void waitForInterruptAvailable ()
    {
        while (!bus.intrptReqAvailable ())
            SimulatorClock::forwardClock (10ms);
    }
};

// Verify that the Read Header function (i.e. the Read function with the FMT
// bit set) returns the sector header. The header contains the cylinder
// address of the sector.
TEST_F (RK11DReadHeaderTest, readHeaderOneSectorSucceeds)
{
    // Read the sector header from sector 0 on cylinder 1 to address 0.
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177777),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 040),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Read) | RKCS_FMT | RKCS_GO),
        StatusCode::Success);

    // The Read Header function is synchronous so the function is ready
    // when the controller is ready.
    waitForControllerReady (rk11dDevice);

    // Verify the sector header has been transferred and no error indicated
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDB}), 040);

    // Sector 12 is on track 1. The cylinder number is encoded in RKDA format
     ASSERT_EQ (bus.read (0), 040);
}


// Verify a Read Header from three consecutive sectors, crossing a track
// boundary
TEST_F (RK11DReadHeaderTest, readHeaderThreeSectorsSucceeds)
{
    // Read three sectors, starting a sector 11, head 1 and cylinder 1
    // (i.e. the last sector on cylinder 1).
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177775),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 073),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Read) | RKCS_FMT | RKCS_GO),
        StatusCode::Success);

    // The Read Header function is synchronous so the function is ready
    // when the controller is ready.
    waitForControllerReady (rk11dDevice);

    // Verify the sector header has been transferred and no error indicated
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDB}), 0100);

    // The three sectors are on cylinders 1 and 2
    ASSERT_EQ (bus.read (0), 040);
    ASSERT_EQ (bus.read (2), 0100);
    ASSERT_EQ (bus.read (4), 0100);
}
