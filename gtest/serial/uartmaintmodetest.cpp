#include "bus/qbus/qbus.h"
#include "configdata/serialconfig/uartconfig/uartconfig.h"
#include "configdata/serialconfig/uarttypeconfig/uarttypeconfig.h"
#include "configdata/serialconfig/consoleconfig/consoleconfig.h"
#include "dl11/uart/uart.h"
#include "busaddress/busaddress.h"
#include "statuscodes.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <gtest/gtest.h>
#include <memory>
#include <chrono>

using std::unique_ptr;
using std::make_unique;

using namespace std::chrono_literals;

class UARTMaintModeTest : public ::testing::Test
{
protected:
    // Use our own set of definitions to avoid common cause issues
    static constexpr u16 UARTBASE = 0177560;
    static constexpr u16 RCSR = UARTBASE;
    static constexpr u16 RBUF = UARTBASE + 02;
    static constexpr u16 XCSR = UARTBASE + 04;
    static constexpr u16 XBUF = UARTBASE + 06;

    // RCSR bit definitions
    static constexpr u16  RCSR_RCVR_DONE  = (1 << 7);

    // XCSR bit definitions
    static constexpr u16  XCSR_MAINT      = (1 << 2);
    static constexpr u16  XCSR_XMIT_READY = (1 << 7);

    void waitForRecvDone (UART *uart)
    {
        u16 result;
        do
        {
            SimulatorClock::forwardClock (100ms);
            uart->read (BusAddress (RCSR, BusAddress::Width::_16Bit), &result);
        }
        while (!(result & RCSR_RCVR_DONE));
    }
};

TEST_F (UARTMaintModeTest, maintenanceModeCanBeSet)
{
    UARTConfig uartConfig {0177560, 060, false};
    ConsoleConfig consoleConfig {};
    Qbus bus;
    u16 data;

    // Power must be Success for the UART receiver to function
    bus.BPOK ().set (true);

    unique_ptr<UART> uart = make_unique<UART> (&bus,
        UARTTypeConfig {.maintenanceModeSupported = true},
        uartConfig, 0, consoleConfig);

    // Set Maintenance Mode
    EXPECT_EQ (uart->writeWord (BusAddress (XCSR, BusAddress::Width::_16Bit), XCSR_MAINT),
        StatusCode::Success);

    // Check Transmitter Ready
    EXPECT_EQ (uart->read (BusAddress (XCSR, BusAddress::Width::_16Bit), &data),
        StatusCode::Success);
    EXPECT_TRUE (data & XCSR_XMIT_READY);

    // Write character to the transmit buffer
    EXPECT_EQ (uart->writeWord (BusAddress (XBUF, BusAddress::Width::_16Bit), '@'),
        StatusCode::Success);

    // Check Receiver Done
    waitForRecvDone (uart.get ());

    // Read the character back
    EXPECT_EQ (uart->read (BusAddress (RBUF, BusAddress::Width::_16Bit), &data),
        StatusCode::Success);
    EXPECT_EQ (data, '@');

    // Halt the simulator's clock to wake up the transmitter from its transmit
    // delay. This is required to allow the destructor to finish the
    // transmitter thread.
    SimulatorClock::halt ();
}