#include "rk/rk11d/rk11d.h"
#include "mem/ms11p/ms11p.h"
#include "bus/unibus/unibus.h"
#include "kt24/kt24.h"
#include "statuscodes.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <gtest/gtest.h>
#include <chrono>

using namespace std::chrono_literals;

// Definition of the test fixture
class RK11DReadTest : public ::testing::Test
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
    static constexpr u16  RKER_NXS = (1 << 5);
    static constexpr u16  RKER_NXC = (1 << 6);
    static constexpr u16  RKER_NXD = (1 << 7);
    static constexpr u16  RKER_OVR = (1 << 14);
    static constexpr u16  RKER_DRE = (1 << 15);

    // RKCS bit definitions
    static constexpr u16  RKCS_GO  = (1 << 0);
    static constexpr u16  RKCS_RDY = (1 << 7);
    static constexpr u16  RKCS_IBA = (1 << 11);
    static constexpr u16  RKCS_HE  = (1 << 14);
    static constexpr u16  RKCS_ERR = (1 << 15);
    inline u16 RKCS_OPERATION (u16 function) { return (function & 7) << 1; }
    constexpr u16 getMexBits (u16 rkcs) { return rkcs & 000060; }

    // Expressions to split a 18-bit address into RKCS Memory Extension bits
    // and a 16-bit RKBA.
    constexpr u16 RKCS_MEX_BITS (u32 address) { return (address >> 16) << 4; }
    constexpr u16 RKBA_BITS (u32 address) { return address & 0177777; }

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

// Before we can verify the read function a sector has to be written first
//
TEST_F (RK11DReadTest, readSectorSucceeds)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] =
        RK05Config (RK05Config
            ({
                .fileName = "rk05.dsk",
                .newFile = true,
                .overwrite = true
            }));

    Unibus bus;
    MS11P ms11p {&bus};
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    // Create a minimal system, consisting of just the bus, memory
    // and the RK11-D/RK05 to be tested.
    bus.installModule (&ms11p);
    bus.installModule (rk11dDevice);

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

    for (u16 address = 0; address < 512; address += 2)
        bus.writeWord (address, 0);

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Read) | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    // Verify all words have been transferred and no error indicated. 
    // The RKDB should contain the last word transferred.
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKBA}), 01000);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDA}), 1);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDB}), 0177777);

    for (u16 contents = 0, address = 0; address < 512; address += 2)
    {
        contents = bus.read (address);
        ASSERT_EQ (contents, 0177777);
    }
}

TEST_F (RK11DReadTest, readOnNonReadyDriveReportsError)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] =
        RK05Config (RK05Config
        ({
            .fileName = "rk05.dsk",
            .newFile = true,
            .overwrite = true,
            .spinUpTime = 100
        }));

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Read) | RKCS_GO), StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}) & RKER_DRE, RKER_DRE);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDS}) & RKDS_DRY, 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}) & (RKCS_ERR | RKCS_HE),
        RKCS_ERR | RKCS_HE);
}

// Attempt to read from sector 12
TEST_F (RK11DReadTest, readFromNonExistentSectorFails)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] =
        RK05Config (RK05Config
        ({
            .fileName = "rk05.dsk",
            .newFile = true,
            .overwrite = true
        }));

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 014),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Read) | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}) & RKER_NXS, RKER_NXS);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}) & (RKCS_ERR | RKCS_HE),
        RKCS_ERR | RKCS_HE);

}

// Attempt to read from cylinder 203
TEST_F (RK11DReadTest, readFromNonExistentCylinderFails)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] =
        RK05Config (RK05Config
        ({
            .fileName = "rk05.dsk",
            .newFile = true,
            .overwrite = true
        }));

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 014540),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Read) | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}) & RKER_NXC, RKER_NXC);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}) & (RKCS_ERR | RKCS_HE),
        RKCS_ERR | RKCS_HE);

}

// Attempt to read from drive 4
TEST_F (RK11DReadTest, readFromNonExistentDriveFails)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] =
        RK05Config (RK05Config
        ({
            .fileName = "rk05.dsk",
            .newFile = true,
            .overwrite = true
        }));

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0100000),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Read) | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}) & RKER_NXD, RKER_NXD);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}) & (RKCS_ERR | RKCS_HE),
        RKCS_ERR | RKCS_HE);
}

// Try to read 512 word starting from the last sector (cylinder 202,
// sector 11, head 1). This should result in an overrun.
TEST_F (RK11DReadTest, readPastEndOfDiskFails)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] =
        RK05Config (RK05Config
        ({
            .fileName = "rk05.dsk",
            .newFile = true,
            .overwrite = true
        }));

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177000),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 014533),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Read) | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}) & RKER_OVR, RKER_OVR);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}) & (RKCS_ERR | RKCS_HE),
        RKCS_ERR | RKCS_HE);
}

// Setting the RKCS IBA bit inhbits the RKBA from incrementing during a normal
// transfer function.
TEST_F (RK11DReadTest, readWithIBASetSucceeds)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] =
        RK05Config (RK05Config
        ({
            .fileName = "rk05.dsk",
            .newFile = true,
            .overwrite = true
        }));

    Unibus bus;
    MS11P ms11p {&bus};
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    // Create a minimal system, consisting of just the bus, memory
    // and the RK11-D/RK05 to be tested.
    bus.installModule (&ms11p);
    bus.installModule (rk11dDevice);

    // Set just the last word of the sector to be written to the pattern
    // to be read back. The rest of the memory is initialized to the value 0.
    bus.writeWord (0776, 0177777);

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

    // Reset the pattern word so the complete memory has value 0
    bus.writeWord (0, 0);

    // Now read 256 words from the first sector with the IBA bit set
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_OPERATION (Operation::Read) | RKCS_IBA | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    // Verify 256 words have been read while the BA hasn't been incremented
    // and no error indicated
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKBA}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDA}), 1);

    // Address zero now should contain the last word of the sector
    ASSERT_EQ (bus.read (0), 0177777);
}

// This test verifies that the RKCS Memory Extension bits are used to
// construct the bus address. To this end a sector is written with the
// contents from a buffer starting at address 0, which is then read back
// at address 01401000.
// 
TEST_F (RK11DReadTest, memoryExtensionBitsAreUsed)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] =
        RK05Config (RK05Config
        ({
            .fileName = "rk05.dsk",
            .newFile = true,
            .overwrite = true
        }));

    Unibus bus;
    MS11P ms11p {&bus};
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    // Create a minimal system, consisting of just the bus, memory
    // and the RK11-D/RK05 to be tested.
    bus.installModule (&ms11p);
    bus.installModule (rk11dDevice);

    static const u32 memoryAddress {01401000};

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

    // Read the sector back at memory address 01400000
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA}, 
        RKBA_BITS (memoryAddress)), StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_MEX_BITS (memoryAddress) | RKCS_OPERATION (Operation::Read) | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    // Verify all words have been transferred and no error indicated
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKBA}),
        RKBA_BITS (memoryAddress) + 01000);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDA}), 1);

    EXPECT_EQ (bus.read (BusAddress {memoryAddress, BusAddress::Width::_18Bit}), 0177777);
    EXPECT_EQ (bus.read (BusAddress {memoryAddress + 510, BusAddress::Width::_18Bit}), 0177777);
}

TEST_F (RK11DReadTest, memoryExtensionBitsAreIncremented)
{
    RK11DConfig rk11dConfig {};
    rk11dConfig.rk05Config[0] =
        RK05Config (RK05Config
        ({
            .fileName = "rk05.dsk",
            .newFile = true,
            .overwrite = true
        }));

    Unibus bus;
    MS11P ms11p {&bus};
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, rk11dConfig);

    // Create a minimal system, consisting of just the bus, memory
    // and the RK11-D/RK05 to be tested.
    bus.installModule (&ms11p);
    bus.installModule (rk11dDevice);

    static u32 memoryAddress {0177000};

    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKWC}, 0177400),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKBA},
        RKBA_BITS (memoryAddress)), StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKDA}, 0),
        StatusCode::Success);
    EXPECT_EQ (rk11dDevice->writeWord (BusAddress {RKCS},
        RKCS_MEX_BITS (memoryAddress) | RKCS_OPERATION (Operation::Read) | RKCS_GO),
        StatusCode::Success);

    waitForControllerReady (rk11dDevice);

    // Verify all words have been transferred and no error indicated
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);

    // Verify the correct bus address is indicated
    memoryAddress += 01000;
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKBA}),
        RKBA_BITS (memoryAddress));
    EXPECT_EQ (getMexBits (rk11dDevice->read (BusAddress {RKCS})),
        RKCS_MEX_BITS (memoryAddress));
}