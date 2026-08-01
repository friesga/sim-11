#include "rk/rk11d/rk11d.h"
#include "mem/ms11p/ms11p.h"
#include "bus/unibus/unibus.h"
#include "statuscodes.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <gtest/gtest.h>
#include <chrono>

using namespace std::chrono_literals;

// Definition of the test fixture
class RK11DWriteCheckTest : public ::testing::Test
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
    constexpr u16 getRKDSdriveId (u16 rkds) { return (rkds >> 13); }

    // RKER bit definitions
    static constexpr u16  RKER_WCE = (1 << 0);
    static constexpr u16  RKER_NXS = (1 << 5);
    static constexpr u16  RKER_NXC = (1 << 6);
    static constexpr u16  RKER_NXD = (1 << 7);
    static constexpr u16  RKER_OVR = (1 << 14);
    static constexpr u16  RKER_DRE = (1 << 15);

    // RKCS bit definitions
    static constexpr u16  RKCS_GO  = (1 << 0);
    static constexpr u16  RKCS_RDY = (1 << 7);
    static constexpr u16  RKCS_IBA  = (1 << 11);
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

        rk11dConfig.numUnits = 1;

        rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

        // Create a minimal system, consisting of just the bus, memory
        // and the RK11-D/RK05 to be tested.
        bus.installModule (&ms11p);
        bus.installModule (rk11dDevice);

        // Power on the RK05 drive. The drive will spin up immediately as the
        // default spin up time is zero.
        bus.BPOK ().set (true);
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
};

TEST_F (RK11DWriteCheckTest, writeCheckReportsNoError)
{
    // Fill the memory's first 512 words with a value to verify that the
    // written sector is read back
    for (u16 address = 0; address < 512; address += 2)
        bus.writeWord (address, 0177777);

    // Write 256 words. Load the word count register with the 2's complement
    // value of 256.
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Write) | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    // Verify all words have been transferred and no error indicated
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKBA}), 01000);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDA}), 1);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDB}), 0177777);

    // Reset the controller to clear the registers
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::ControlReset) | RKCS_GO),
        StatusCode::Success);
    waitForControllerReady (rk11dDevice);

    // Execute the Write Check
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::WriteCheck) | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKBA}), 01000);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDA}), 1);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDB}), 0177777);
}

TEST_F (RK11DWriteCheckTest, writeCheckReportsError)
{
    // Fill the memory's first 512 words with a value to verify that the
    // written sector is read back
    for (u16 address = 0; address < 512; address += 2)
        bus.writeWord (address, 0177777);

    // Write 256 words. Load the word count register with the 2's complement
    // value of 256.
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Write) | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    bus.writeWord (510, 0);

    // Verify all words have been transferred and no error indicated
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKBA}), 01000);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDA}), 1);

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::WriteCheck) | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), RKER_WCE);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKBA}), 01000);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDA}), 1);
}


TEST_F (RK11DWriteCheckTest, writeCheckWithIBASet)
{
    // Fill the memory's first 512 words with a value that will then be
    // checked by a write check
    for (u16 address = 0; address < 512; address += 2)
        bus.writeWord (address, 0177777);

    // Write 256 words. Load the word count register with the 2's complement
    // value of 256.
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Write) | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    // Verify all words have been transferred and no error indicated
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKBA}), 01000);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDA}), 1);

    // Check the written sector contains the pattern a address 0
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::WriteCheck) | RKCS_IBA | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKBA}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDA}), 1);
}
