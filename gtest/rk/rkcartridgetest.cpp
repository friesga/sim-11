#include "pdp11/pdp11.h"
#include "bus/unibus/unibus.h"
#include "disk/rk/rk11d/rk11d.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "chrono/simulatorclock/simulatorclock.h"
#include "../fakesdl/audio/fakeaudiosystem/fakeaudiosystem.h"
#include "disk/cartridge/cartridge.h"
#include "fakecartridgehandler.h"

#include <gtest/gtest.h>
#include <stdio.h>
#include <chrono>
#include <memory>


using namespace std::chrono_literals;
using std::unique_ptr;

// Definition of the test fixture
class RK11DCartridgeTest : public ::testing::Test
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

    // This function powers on the given drive. This could be acomplished by
    // setting the Unibus BPOK true, but that turns the drive on via the event
    // queue, causing thread scheduling issues. The RK11DCartridgeTest class
    // has to be as a friend class of the RK05 class for this function.
    void powerOn (RK05* drive)
    {
        return drive->stateMachine_->dispatch (RK05::PowerOn {});
    }
};

TEST_F (RK11DCartridgeTest, cartridgeIsLoaded)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] = RK05Config (RK05Config {});
    rk11dConfig.rk05Config[0]->fileName = "scratch.img";

    Unibus bus;
    FakeAudioSystem fakeAudioSystem {};

    // Define a CartridgeHandlerFactory to create a FakeCartridgeHandler
	CartridgeHandlerFactory factory = [] (DiskDrive* drive)
	{
		return std::make_unique<FakeCartridgeHandler> (drive);
	};

    // Create a new disk image file to be used as a cartridge
    FILE* file = std::fopen ("scratch.img", "wb+");
    EXPECT_TRUE (file != nullptr);

    RK11D* rk11dDevice = new RK11D (&bus, nullptr, &fakeAudioSystem, factory,
        rk11dConfig);

    powerOn (rk11dDevice->unit (0));

    // Verify the drive reports a cartridge loaded
    EXPECT_FALSE (rk11dDevice->unit (0)->noCartridgeLoaded ());
}

TEST_F (RK11DCartridgeTest, cartridgeCanBeLoaded)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] = RK05Config (RK05Config {});
    rk11dConfig.rk05Config[0]->spinUpTime = 100;

    Unibus bus;
    FakeAudioSystem fakeAudioSystem {};

    // Define a CartridgeHandlerFactory to create a FakeCartridgeHandler
	CartridgeHandlerFactory factory = [] (DiskDrive* drive)
	{
		return std::make_unique<FakeCartridgeHandler> (drive);
	};

    RK11D* rk11dDevice = new RK11D (&bus, nullptr, &fakeAudioSystem, factory,
        rk11dConfig);

    powerOn (rk11dDevice->unit (0));

    EXPECT_TRUE (rk11dDevice->unit (0)->noCartridgeLoaded ());

    // Create a new disk image file to be used as a cartridge
    FILE* file = std::fopen ("scratch.img", "wb+");
    EXPECT_TRUE (file != nullptr);

    EXPECT_TRUE (rk11dDevice->unit (0)->loadCartridge ("scratch.img"));

     // Verify the drive reports a cartridge loaded
    EXPECT_FALSE (rk11dDevice->unit (0)->noCartridgeLoaded ());
}

TEST_F (RK11DCartridgeTest, nonExistingFileThrows)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] = RK05Config (RK05Config {});
    rk11dConfig.rk05Config[0]->spinUpTime = 100;
    rk11dConfig.rk05Config[0]->fileName = "non-existing-file.img";

    Unibus bus;
    FakeAudioSystem fakeAudioSystem {};

    // Define a CartridgeHandlerFactory to create a FakeCartridgeHandler
	CartridgeHandlerFactory factory = [] (DiskDrive* drive)
	{
		return std::make_unique<FakeCartridgeHandler> (drive);
	};

    try
    {
        RK11D* rk11dDevice = new RK11D (&bus, nullptr, &fakeAudioSystem, factory,
            rk11dConfig);
        FAIL ();
    }
    catch (std::invalid_argument const& except)
    {
        EXPECT_STREQ (except.what (), "Cannot create cartridge non-existing-file.img");
    }
    catch (...)
    {
        FAIL ();
    }
}

TEST_F (RK11DCartridgeTest, cartridgeCannotBeLoaded)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] = RK05Config (RK05Config {});
    rk11dConfig.rk05Config[0]->spinUpTime = 100;

    Unibus bus;
    FakeAudioSystem fakeAudioSystem {};

    // Define a CartridgeHandlerFactory to create a FakeCartridgeHandler
	CartridgeHandlerFactory factory = [] (DiskDrive* drive)
	{
		return std::make_unique<FakeCartridgeHandler> (drive);
	};

    RK11D* rk11dDevice = new RK11D (&bus, nullptr, &fakeAudioSystem, factory,
        rk11dConfig);

    powerOn (rk11dDevice->unit (0));

    EXPECT_FALSE (rk11dDevice->unit (0)->loadCartridge ("non-existing-file.img"));
}

TEST_F (RK11DCartridgeTest, loadCartridgeFails)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] = RK05Config (RK05Config {});
    rk11dConfig.rk05Config[0]->fileName = "scratch.img";

    Unibus bus;
    FakeAudioSystem fakeAudioSystem {};

    // Define a CartridgeHandlerFactory to create a FakeCartridgeHandler
	CartridgeHandlerFactory factory = [] (DiskDrive* drive)
	{
		return std::make_unique<FakeCartridgeHandler> (drive);
	};

    // Create a new disk image file to be used as a cartridge
    FILE* file = std::fopen ("scratch.img", "wb+");
    EXPECT_TRUE (file != nullptr);

    RK11D* rk11dDevice = new RK11D (&bus, nullptr, &fakeAudioSystem, factory,
        rk11dConfig);

    powerOn (rk11dDevice->unit (0));

    // Verify the drive reports a cartridge loaded
    EXPECT_FALSE (rk11dDevice->unit (0)->noCartridgeLoaded ());

    // Loading a cartridge while a cartridge is loaded should fail
    EXPECT_FALSE (rk11dDevice->unit (0)->loadCartridge ("scratch.img"));
}

TEST_F (RK11DCartridgeTest, cartridgeCanNotBeUnloaded)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] = RK05Config (RK05Config {});
    rk11dConfig.rk05Config[0]->fileName = "scratch.img";

    Unibus bus;
    FakeAudioSystem fakeAudioSystem {};

    // Define a CartridgeHandlerFactory to create a FakeCartridgeHandler
	CartridgeHandlerFactory factory = [] (DiskDrive* drive)
	{
		return std::make_unique<FakeCartridgeHandler> (drive);
	};

    // Create a new disk image file to be used as a cartridge
    FILE* file = std::fopen ("scratch.img", "wb+");
    EXPECT_TRUE (file != nullptr);

    RK11D* rk11dDevice = new RK11D (&bus, nullptr, &fakeAudioSystem, factory,
        rk11dConfig);

    powerOn (rk11dDevice->unit (0));

    // Verify the drive reports a cartridge loaded
    EXPECT_FALSE (rk11dDevice->unit (0)->noCartridgeLoaded ());

    // Try to unload the cartridge. This should fail as the drive is spun up
    // because the LOAD/RUN switch is in the upper (RUN) position.
    EXPECT_FALSE (rk11dDevice->unit (0)->unloadCartridge ());
}
