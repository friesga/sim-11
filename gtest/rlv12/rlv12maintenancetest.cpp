#include "pdp11/pdp11.h"
#include "msv11d/msv11d.h"
#include "rlv12/rlv12.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <gtest/gtest.h>
#include <memory>
#include <chrono>

using namespace std::chrono_literals;

// Definition of the test fixture
class RLV12MaintenanceTest : public ::testing::Test
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
    static constexpr u16  CSR_ControllerReady     = (1 << 7);

    // Create bus structure, an RLV12 device and install the device
    Qbus bus;
    MSV11D *msv11 = new MSV11D (&bus);
    RLV12 *rlv12Device = new RLV12 (&bus);

    void SetUp() override
    {
        // Create a minimal system, conisting of just the bus, memory
        // and the RLV12 device to be tested.
        bus.installModule (msv11);
        bus.installModule (rlv12Device);
    }

    void waitForControllerReady ()
    {
        u16 result {0};
        do
        {
            SimulatorClock::forwardClock (100ms);
            rlv12Device->read (RLCSR, &result);
        }
        while (!(result & CSR_ControllerReady));
    }
};


TEST_F (RLV12MaintenanceTest, maintenance)
{
    u16 const MaintenanceMode = 0;

    // Make sure the controller has started
    waitForControllerReady ();

    // Set memory address, word count (-511) and issue maintenance
    // command.
    // The command is executed with no units attached as attaching a
    // unit will cause a Volume Check condition which results in error
    // bits set.
    rlv12Device->writeWord (RLBAR, 0);
    rlv12Device->writeWord (RLMPR, 0177001);
    rlv12Device->writeWord (RLCSR, MaintenanceMode);

    waitForControllerReady ();

    // Expected result: CSR CRDY (bit 7) set, all error bits cleared,
    // Drive Ready ckeared (as no unit is attached).
    u16 result;
    rlv12Device->read (RLCSR, &result);
    ASSERT_EQ (result, 0200);
}