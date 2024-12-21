#include "pdp11/pdp11.h"
#include "msv11d/msv11d.h"
#include "rlv12/rlv12.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <gtest/gtest.h>
#include <memory>
#include <chrono>

using std::make_shared;
using namespace std::chrono_literals;

// Write to unit tests.

// Definition of the test fixture
class RLV12VolumeCheck : public ::testing::Test
{
protected:
    // Use our own set of definitions to avoid common cause issues
    static constexpr u16 RLBASE = 0174400;
    static constexpr u16 RLCSR = RLBASE;
    static constexpr u16 RLBAR = RLBASE + 02;
    static constexpr u16 RLDAR = RLBASE + 04;
    static constexpr u16 RLMPR = RLBASE + 06;
    // A 16 bit system has no BAE register

    // CSR bit definitions
    static constexpr u16  CSR_GetStatusCommand    = (2 << 1);
    static constexpr u16  CSR_ReadDataCommand     = (06 << 1);
    static constexpr u16  CSR_DriveReady          = (1 << 0);
    static constexpr u16  CSR_ControllerReady     = (1 << 7);
    static constexpr u16  CSR_OperationIncomplete = (1 << 10);
    static constexpr u16  CSR_DriveError          = (1 << 14); 
    static constexpr u16  CSR_CompositeError      = (1 << 15); 
    static constexpr u16  CSR_Drive0              = 0;
    static constexpr u16  CSR_Drive1              = (1 << 8);
    static constexpr u16  CSR_Drive2              = (2 << 8);
    static constexpr u16  CSR_Drive3              = (3 << 8);

    // DAR bit definitions
    static constexpr u16 DAR_Marker               = (1 << 0);
    static constexpr u16 DAR_GetStatus            = (1 << 1);
    static constexpr u16 DAR_Reset                = (1 << 3);

    // MPR bit definitions
    static constexpr u16 MPR_BrushHome            = (1 << 3);
    static constexpr u16 MPR_HeadsOut             = (1 << 4);
    static constexpr u16 MPR_DriveType            = (1 << 7);
    static constexpr u16 MPR_DriveSelectError     = (1 << 8);
    static constexpr u16 MPR_VolumeCheck          = (1 << 9);
    static constexpr u16 MPR_WriteLock            = (1 << 13);
    static const u16 MPR_LockOn                   = 5;
    

    // Create bus structure, an RLV12 device and install the device
    Qbus bus;
    MSV11D *msv11 = new MSV11D (&bus);
    RLV12 *rlv12Device = new RLV12 (&bus);

    void waitForControllerReady ()
    {
        u16 result;
        do
        {
            SimulatorClock::forwardClock (100ms);
            rlv12Device->read (RLCSR, &result);
        }
        while (!(result & CSR_ControllerReady));
    }

    void SetUp() override
    {
        // Create a minimal system, conisting of just the bus, memory
        // and the RLV12 device to be tested.
        bus.installModule (msv11);
        bus.installModule (rlv12Device);

        // Make sure the controller has started
        waitForControllerReady ();
    }
};

TEST_F (RLV12VolumeCheck, volumeCheckReported)
{
    // This unit configuration uses the default spin_up_time of zero seconds
    // so the drive immediately locks on to cylinder 0.
    RLUnitConfig rlUnitConfig
    ({
        .fileName = "rl01.dsk",
        .newFile = true,
        .overwrite = true,
        .writeProtect = true,
        .unitType = RLUnitConfig::RLUnitType::RL01
        });

    // Attach a new disk to unit 0
    ASSERT_EQ (rlv12Device->unit (0)->init (make_shared<RLUnitConfig> (rlUnitConfig)),
        StatusCode::OK);

    // Verify the controller is ready to perform an operation (the drive
    // does not have to be ready)
    waitForControllerReady ();

    // Load DAR with ones in bits 01 and 00, reset bit set and
    // zeros in the other locations
    rlv12Device->writeWord (RLDAR, DAR_GetStatus | DAR_Marker);

    // Load the CSR with drive-select bits for unit 3, a negative GO bit
    // (i.e. bit 7 cleared), interrups disabled and a Get Status Command (02)
    // in the function bits.
    rlv12Device->writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive0);

    waitForControllerReady ();

    // Expected volume check reported in the MPR register
    u16 mpr;
    rlv12Device->read (RLMPR, &mpr);
    ASSERT_EQ (mpr & MPR_VolumeCheck, MPR_VolumeCheck);
}

TEST_F (RLV12VolumeCheck, readDataWithVolumeCheckReportsDriveError)
{
    // This unit configuration uses the default spin_up_time of zero seconds
    // so the drive immediately locks on to cylinder 0.
    RLUnitConfig rlUnitConfig
    ({
        .fileName = "rl01.dsk",
        .newFile = true,
        .overwrite = true,
        .writeProtect = true,
        .unitType = RLUnitConfig::RLUnitType::RL01
        });

    // Attach a new disk to unit 0
    ASSERT_EQ (rlv12Device->unit (0)->init (make_shared<RLUnitConfig> (rlUnitConfig)),
        StatusCode::OK);

    // Verify the controller is ready to perform an operation (the drive
    // does not have to be ready)
    waitForControllerReady ();

    // Try to read data from the drive.
    // Point at memory address 0
    rlv12Device->writeWord (RLBAR, 0);

    // Load DAR with disk address zero
    rlv12Device->writeWord (RLDAR, 0);

    // Load MPR with two's complement for 256 words
    rlv12Device->writeWord (RLMPR, 0xFF00);

    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrupts disabled and a Read Data Command (06)
    // in the function bits.
    rlv12Device->writeWord (RLCSR, CSR_ReadDataCommand);

    waitForControllerReady ();

    // Verify the controller reports a drive error
    u16 csr;
    rlv12Device->read (RLCSR, &csr);
    ASSERT_EQ (csr & (CSR_ControllerReady | CSR_DriveReady | 
        CSR_DriveError | CSR_CompositeError),
        CSR_ControllerReady | CSR_DriveReady |
        CSR_DriveError | CSR_CompositeError);
}

TEST_F (RLV12VolumeCheck, volumeCheckIsReset)
{
    // This unit configuration uses the default spin_up_time of zero seconds
    // so the drive immediately locks on to cylinder 0.
    RLUnitConfig rlUnitConfig
    ({
        .fileName = "rl01.dsk",
        .newFile = true,
        .overwrite = true,
        .writeProtect = true,
        .unitType = RLUnitConfig::RLUnitType::RL01
        });

    // Attach a new disk to unit 0
    ASSERT_EQ (rlv12Device->unit (0)->init (make_shared<RLUnitConfig> (rlUnitConfig)),
        StatusCode::OK);

    // Verify the controller is ready to perform an operation (the drive
    // does not have to be ready)
    waitForControllerReady ();

    // Load DAR with ones in bits 01 and 00, reset bit set and
    // zeros in the other locations
    rlv12Device->writeWord (RLDAR, DAR_Reset | DAR_GetStatus | DAR_Marker);

    // Load the CSR with drive-select bits for unit 3, a negative GO bit
    // (i.e. bit 7 cleared), interrups disabled and a Get Status Command (02)
    // in the function bits.
    rlv12Device->writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive0);

    waitForControllerReady ();

    // Expected no volume check reported in the MPR register
    u16 mpr;
    rlv12Device->read (RLMPR, &mpr);
    ASSERT_EQ (mpr & MPR_VolumeCheck, 0);
}


TEST_F (RLV12VolumeCheck, noDataReadOnVolumeCheck)
{
    // This unit configuration uses the default spin_up_time of zero seconds
    // so the drive immediately locks on to cylinder 0.
    RLUnitConfig rlUnitConfig
    ({
        .fileName = "rl01.dsk",
        .newFile = true,
        .overwrite = true,
        .writeProtect = true,
        .unitType = RLUnitConfig::RLUnitType::RL01
        });

    // Attach a new disk to unit 0
    ASSERT_EQ (rlv12Device->unit (0)->init (make_shared<RLUnitConfig> (rlUnitConfig)),
        StatusCode::OK);

    // Fill 512 bytes of memory with the value 0xff and write a mark at
    // address 512 to be able to verify the memory is not overwritten by the
    // Read Data command
    u16 address;
    for (address = 0; address < 512; address += 2)
        bus.writeWord (address, 0177777);
    bus.writeWord (address, 01);

    // Verify the controller is ready to perform an operation (the drive
    // does not have to be ready)
    waitForControllerReady ();

    // Try to read data from the drive.
    // Point at memory address 0
    rlv12Device->writeWord (RLBAR, 0);

    // Load DAR with disk address zero
    rlv12Device->writeWord (RLDAR, 0);

    // Load MPR with two's complement for 256 words
    rlv12Device->writeWord (RLMPR, 0xFF00);

    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrupts disabled and a Read Data Command (06)
    // in the function bits.
    rlv12Device->writeWord (RLCSR, CSR_ReadDataCommand);

    waitForControllerReady ();

    // Verify the controller reports a drive error
    u16 csr;
    rlv12Device->read (RLCSR, &csr);
    ASSERT_EQ (csr & (CSR_ControllerReady | CSR_DriveReady |
        CSR_DriveError | CSR_CompositeError),
        CSR_ControllerReady | CSR_DriveReady |
        CSR_DriveError | CSR_CompositeError);

    // Verify the Read Data command hasn't failed and the first 512 bytes
    // of memory are overwritten.
    u16 contents;
    for (address = 0; address < 512; address += 2)
    {
        contents = bus.read (address);
        ASSERT_EQ (contents, 0177777);
    }
}