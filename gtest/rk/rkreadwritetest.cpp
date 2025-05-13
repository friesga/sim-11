#include "rk/rk11d/rk11d.h"
#include "ms11p/ms11p.h"
#include "bus/unibus/unibus.h"
#include "statuscodes.h"

#include <gtest/gtest.h>
#include <memory>

using std::make_shared;

// Definition of the test fixture
class RK11DReadwriteTest : public ::testing::Test
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
    static constexpr u16  RKER_NXD = (1 << 7);

    // RKCS bit definitions
    static constexpr u16  RKCS_GO = (1 << 0);
    static constexpr u16  RKCS_RDY = (1 << 7);
    inline u16 RKCS_FUNCTON (u16 function) { return (function & 7) << 1; }


    // Function definitions
    enum Function
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
        } while (!(result & RKCS_RDY));
    }

    void waitForDriveReady (RK11D* controller, u16 driveId)
    {
        u16 result;
        do
        {
            // SimulatorClock::forwardClock (100ms);
            result = controller->read (RKDS);
        } while (!((result & RKDS_DRY) && getRKDSdriveId (result) == driveId));

    }
};


TEST_F (RK11DReadwriteTest, readWriteSector)
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
    MS11P ms11p {&bus};
    RK11D* rk11dDevice = new RK11D (&bus, nullptr,
        make_shared<RK11DConfig> (rk11dConfig));

    // Create a minimal system, consisting of just the bus, memory
    // and the RK11-D/RK05 to be tested.
    bus.installModule (&ms11p);
    bus.installModule (rk11dDevice);

    // Fill the memory's first 512 words with a value to verify that the
    // written sector is read back
    for (u16 address = 0; address < 512; address += 2)
        bus.writeWord (address, 0177777);

    // Load the word count register with the 2's complement value of 256
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_FUNCTON (Function::Write) | RKCS_GO),
        StatusCode::Success);

    for (u16 address = 0; address < 512; address += 2)
        bus.writeWord (address, 0);

    waitForDriveReady (rk11dDevice, 0);

    // Verify all words have been transferred and no error indicated
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_FUNCTON (Function::Read) | RKCS_GO),
        StatusCode::Success);

    for (u16 contents = 0, address = 0; address < 512; address += 2)
    {
        contents = bus.read (address);
        ASSERT_EQ (contents, 0177777);
    }
}