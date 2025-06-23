#include "rk/rk11d/rk11d.h"
#include "ms11p/ms11p.h"
#include "bus/unibus/unibus.h"
#include "statuscodes.h"
#include "chrono/simulatorclock/simulatorclock.h"
#include "bus/interruptrequest/interruptrequest.h"

#include <gtest/gtest.h>
#include <memory>
#include <chrono>

using std::make_shared;

using namespace std::chrono_literals;

// Definition of the test fixture
class RK11DSeekTest : public ::testing::Test
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
    inline u16 getRKDSdriveId (u16 rkds) { return (rkds >> 13); }

    // RKER bit definitions
    static constexpr u16  RKER_NXD = (1 << 7);

    // RKCS bit definitions
    static constexpr u16  RKCS_GO = (1 << 0);
    static constexpr u16  RKCS_RDY = (1 << 7);
    static constexpr u16  RKCS_IDE = (1 << 6);
    static constexpr u16  RKCS_HE = (1 << 14);
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
            make_shared<RK05Config> (RK05Config
            ({
                .unitNumber = 0,
                .fileName = "rk05.dsk",
                .newFile = true,
                .overwrite = true
                }));

        rk11dConfig.rk05Config[1] =
            make_shared<RK05Config> (RK05Config
            ({
                .unitNumber = 1,
                .fileName = "rk05-1.dsk",
                .newFile = true,
                .overwrite = true
                }));

        rk11dConfig.numUnits = 2;

        rk11dDevice = new RK11D (&bus, nullptr,
            make_shared<RK11DConfig> (rk11dConfig));

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

    void waitForInterruptAvailable ()
    {
        while (!bus.intrptReqAvailable ())
            SimulatorClock::forwardClock (10ms);
    }
};


TEST_F (RK11DSeekTest, seekToNonExistentCylinder)
{
    // Try to seek to cylinder 203
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0014540),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Seek) | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    // Verify the RKCS and RKER indicate the error
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}) & (RKCS_ERR | RKCS_HE),
        RKCS_ERR | RKCS_HE);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}) & RKER_NXC, RKER_NXC);
}

TEST_F (RK11DSeekTest, seekToExistentCylinder)
{
    // Try to seek to cylinder 1
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0000040),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Seek) | RKCS_GO),
        StatusCode::Success);

    waitForDriveReady (rk11dDevice, 0);

    // Verify no error and correct status indicated
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDS}),
        RKDS_SC_SA | RKDS_RWS_READY | RKDS_DRY | RKDS_SOK | RKDS_RK05);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}) & (RKCS_ERR | RKCS_HE), 0);
}


TEST_F (RK11DSeekTest, seekGeneratesInterrupts)
{
    // Try to seek to cylinder 202 ond drive 0 to make sure that seeking
    // takes more time than issueing a function to the controller
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0014500),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Seek) | RKCS_IDE | RKCS_GO),
        StatusCode::Success);

    // The acceptance of the Seek function should generate an interrupt request
    waitForControllerReady (rk11dDevice);
    EXPECT_TRUE (bus.intrptReqAvailable ());
    EXPECT_TRUE (bus.containsInterrupt (TrapPriority::BR5, 5, 0));
    InterruptRequest ir;
    EXPECT_TRUE (bus.getIntrptReq (ir));

    // After completion of the seek another interrupt request should be
    // generated and the drive should be ready
    waitForDriveReady (rk11dDevice, 0);
    waitForInterruptAvailable ();

    // Verify no error and correct status indicated
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDS}),
        RKDS_SC_SA | RKDS_RWS_READY | RKDS_DRY | RKDS_SOK | RKDS_RK05);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}) & (RKCS_ERR | RKCS_HE), 0);
}

TEST_F (RK11DSeekTest, overlappedSeeks)
{
    // Try to seek to cylinder 202 to make sure that seeking takes more time
    // than issueing a function to the controller
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0014500),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Seek) | RKCS_IDE | RKCS_GO),
        StatusCode::Success);

    // The acceptance of the Seek function should generate an interrupt request
    waitForControllerReady (rk11dDevice);
    EXPECT_TRUE (bus.intrptReqAvailable ());
    EXPECT_TRUE (bus.containsInterrupt (TrapPriority::BR5, 5, 0));
    InterruptRequest ir;
    EXPECT_TRUE (bus.getIntrptReq (ir));

    // A seek on drive 1 to sector 202 should be accepted
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0034500),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Seek) | RKCS_IDE | RKCS_GO),
        StatusCode::Success);

    // The acceptance of the Seek function should generate an interrupt request.
    waitForControllerReady (rk11dDevice);
    EXPECT_TRUE (bus.intrptReqAvailable ());
    EXPECT_TRUE (bus.containsInterrupt (TrapPriority::BR5, 5, 0));
    EXPECT_TRUE (bus.getIntrptReq (ir));

    // After completion of the first seek an interrupt should be available.
    // This interrupt request is granted, after which an interrupt for
    // completion of the second seek should be available.
    // The RKDS should contain the drive number for which the interrupt was
    // generated.
    waitForInterruptAvailable ();
    EXPECT_TRUE (bus.getIntrptReq (ir));
    u16 firstDrive = getRKDSdriveId (rk11dDevice->read (BusAddress {RKDS}));

    waitForInterruptAvailable ();
    EXPECT_TRUE (bus.getIntrptReq (ir));
    u16 secondDrive = getRKDSdriveId (rk11dDevice->read (BusAddress {RKDS}));

    // Interrupts should be generated for drive 0 and drive 1
    EXPECT_TRUE ((firstDrive == 0 && secondDrive == 1) ||
        (firstDrive == 1 && secondDrive == 0));

    // Verify no error and correct status indicated
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDS}) & 
        RKDS_SC_SA | RKDS_RWS_READY | RKDS_DRY | RKDS_SOK | RKDS_RK05,
        RKDS_SC_SA | RKDS_RWS_READY | RKDS_DRY | RKDS_SOK | RKDS_RK05);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}) & (RKCS_ERR | RKCS_HE), 0);
}

#if 0
// We would like to verify that a transfer function stops the hardware
// poll. This functionality cannot be verified reliable in a test however
// as the outcome depends on thread scheduling (the rk05 drive threads might
// be executed before the transfer command is executed).
TEST_F (RK11DSeekTest, transferFunctionCancelsSeekInterrupts)
{
    InterruptRequest ir;

    // Start a seek on drive 0
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0014500),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Seek) | RKCS_IDE | RKCS_GO),
        StatusCode::Success);

    // The acceptance of the Seek function should generate an interrupt request
    waitForInterruptAvailable ();
    EXPECT_TRUE (bus.getIntrptReq (ir));

    // Start a seek on drive 1
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0034500),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Seek) | RKCS_IDE | RKCS_GO),
        StatusCode::Success);

    // The acceptance of the Seek function should generate an interrupt request
    waitForInterruptAvailable ();
    EXPECT_TRUE (bus.getIntrptReq (ir));

    // Start a read on drive 0
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Read) | RKCS_GO),
        StatusCode::Success);

    // No interrupts should be generated any more while the seeks are
    // completed (the seeks take 85 msec).
    SimulatorClock::forwardClock (100ms);
    EXPECT_FALSE (bus.intrptReqAvailable ());
}
#endif