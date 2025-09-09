#include "rk/rk11d/rk11d.h"
#include "bus/unibus/unibus.h"
#include "statuscodes.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <gtest/gtest.h>
#include <memory>
#include <chrono>

using std::make_shared;
using std::chrono_literals::operator""ms;

// Definition of the test fixture
class RK11DRegistersTest : public ::testing::Test
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
    static constexpr u16 RKMR = RK11D_BASE + 014;
    static constexpr u16 RKDB = RK11D_BASE + 016;

    // RKDS bit definitions
    static constexpr u16  RKDS_DRY = (1 << 7);
    static constexpr u16  RKDS_IDE = (1 << 6);
    static constexpr u16  RKDS_EXB = (1 << 9);
    constexpr u16 getRKDSdriveId (u16 rkds) { return (rkds >> 13); }
    constexpr u16 sectorCounter (u16 rkds) { return (rkds & 017); }

    // RKER bit definitions
    static constexpr u16  RKER_NXD = (1 << 7);

    // RKCS bit definitions
    static constexpr u16  RKCS_GO = (1 << 0);
    static constexpr u16  RKCS_RDY = (1 << 7);
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
            // SimulatorClock::forwardClock (100ms);
            result = controller->read (RKCS);
        }
        while (!(result & RKCS_RDY));
    }

    void waitForDriveReady (RK11D* controller, u16 driveId)
    {
        u16 result;
        do
        {
            // SimulatorClock::forwardClock (100ms);
            result = controller->read (RKDS);
        }
        while (!((result & RKDS_DRY) && getRKDSdriveId (result) == driveId));
        
    }
};


TEST_F (RK11DRegistersTest, controllerIsResponsibleForDefaultAddress)
{
    RK11DConfig rk11dConfig {};

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    // Check that the controller is responsible for its base address
    BusAddress busAddress {RKDS};
    EXPECT_TRUE (rk11dDevice->responsible (busAddress));

    // Check that the controller is not responsible for other addresses
    busAddress = BusAddress {RKDS - 2};
    EXPECT_FALSE (rk11dDevice->responsible (busAddress));

    busAddress = BusAddress {RKDB + 020};
    EXPECT_FALSE (rk11dDevice->responsible (busAddress));
}

// Verify the registers are correcly initialised
TEST_F (RK11DRegistersTest, registersInitialised)
{
    RK11DConfig rk11dConfig {};

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}), RKCS_RDY);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKBA}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDB}), 0);
}

// Verify the RDY remains set when IDE is set. This resembles ZRKJE0 test 5.
TEST_F (RK11DRegistersTest, rdyRemainsSet)
{
    RK11DConfig rk11dConfig {};

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}), RKCS_RDY);

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS}, RKCS_RDY | RKDS_IDE),
        StatusCode::Success);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}),
        RKCS_RDY | RKDS_IDE);
}

TEST_F (RK11DRegistersTest, exbBitIsReadWrite)
{
    RK11DConfig rk11dConfig {};

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}), RKCS_RDY);

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS}, RKDS_EXB),
        StatusCode::Success);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}),
        RKCS_RDY | RKDS_EXB);
}

TEST_F (RK11DRegistersTest, rkmrReadsAsZero)
{
    RK11DConfig rk11dConfig {};

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKMR}), 0);
}

// Write to the maintenance register are discarded
TEST_F (RK11DRegistersTest, rkmrCanBeWritten)
{
    RK11DConfig rk11dConfig {};

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKMR}, 0177777),
        StatusCode::Success);
    
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKMR}), 0);
}

// Verify that a Control Reset function resets the registers and sets the
// controller ready bit.
TEST_F (RK11DRegistersTest, controlResetFunction)
{
    RK11DConfig rk11dConfig {};

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177777),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0177777),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDB}, 0177777),
        StatusCode::Success);

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (ControlReset) | RKCS_GO), StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}), RKCS_RDY);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKBA}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDA}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDB}), 0);
}

// Verify that a function for a not-connected drive returns a drive error
TEST_F (RK11DRegistersTest, nonExistingDriveReturnsError)
{
    RK11DConfig rk11dConfig {};
    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS}, RKCS_OPERATION (DriveReset) | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}) & RKER_NXD, RKER_NXD);
}

TEST_F (RK11DRegistersTest, rkdaCenBeAssignedTo)
{
    RKTypes::RKDA rkda {020000};
    EXPECT_EQ (rkda.driveSelect, 1);

    rkda = DiskAddress {1, 0, 0};
    EXPECT_EQ (rkda.value, 020001);

    rkda = DiskAddress {0, 1, 0};
    EXPECT_EQ (rkda.value, 020020);


    rkda = DiskAddress {0, 0, 1};
    EXPECT_EQ (rkda.value, 020040);
}

TEST_F (RK11DRegistersTest, rkdaCanBeAdded)
{
    RKTypes::RKDA rkda {0160000};
    EXPECT_EQ (rkda.driveSelect, 7);

    rkda = rkda + 1;
    EXPECT_EQ (rkda.driveSelect, 7);
    EXPECT_EQ (rkda.sectorAddress, 1);

    rkda.value = 0160000;
    rkda = rkda + 12;
    EXPECT_EQ (rkda.driveSelect, 7);
    EXPECT_EQ (rkda.sectorAddress, 0);
    EXPECT_EQ (rkda.surface, 1);

    rkda.value = 0160000;
    rkda = rkda + 24;
    EXPECT_EQ (rkda.driveSelect, 7);
    EXPECT_EQ (rkda.sectorAddress, 0);
    EXPECT_EQ (rkda.surface, 0);
    EXPECT_EQ (rkda.cylinderAddress, 1);
}

TEST_F (RK11DRegistersTest, rkdaCanBeAddedTo)
{
    RKTypes::RKDA rkda {0160000};
    EXPECT_EQ (rkda.driveSelect, 7);

    rkda += 1;
    EXPECT_EQ (rkda.driveSelect, 7);
    EXPECT_EQ (rkda.sectorAddress, 1);

    rkda.value = 0160000;
    rkda += 12;
    EXPECT_EQ (rkda.driveSelect, 7);
    EXPECT_EQ (rkda.sectorAddress, 0);
    EXPECT_EQ (rkda.surface, 1);

    rkda.value = 0160000;
    rkda += 24;
    EXPECT_EQ (rkda.driveSelect, 7);
    EXPECT_EQ (rkda.sectorAddress, 0);
    EXPECT_EQ (rkda.surface, 0);
    EXPECT_EQ (rkda.cylinderAddress, 1);
}


// Verify the sector counter in the RKDS is incremented every 3.3 ms.
TEST_F (RK11DRegistersTest, sectorCounterIsIncremented)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] =
        make_shared<RK05Config> (RK05Config
        ({
            .fileName = "rk05.dsk",
            .newFile = true,
            .overwrite = true
            }));

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    // Start at time point 0
    SimulatorClock::reset ();

    EXPECT_EQ (sectorCounter (rk11dDevice->read (BusAddress {RKDS})), 0);

    SimulatorClock::forwardClock (4ms);
    EXPECT_EQ (sectorCounter (rk11dDevice->read (BusAddress {RKDS})), 1);

    SimulatorClock::forwardClock (4ms);
    EXPECT_EQ (sectorCounter (rk11dDevice->read (BusAddress {RKDS})), 2);

    SimulatorClock::forwardClock (28ms);
    EXPECT_EQ (sectorCounter (rk11dDevice->read (BusAddress {RKDS})), 10);

    SimulatorClock::forwardClock (2ms);
    EXPECT_EQ (sectorCounter (rk11dDevice->read (BusAddress {RKDS})), 11);

    SimulatorClock::forwardClock (2ms);
    EXPECT_EQ (sectorCounter (rk11dDevice->read (BusAddress {RKDS})), 0);
}