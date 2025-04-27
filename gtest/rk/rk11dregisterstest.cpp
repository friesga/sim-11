#include "rk/rk11d/rk11d.h"
#include "bus/unibus/unibus.h"

#include <gtest/gtest.h>
#include <memory>

using std::make_shared;

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
    static constexpr u16 RKDB = RK11D_BASE + 016;

    // RKCS bit definitions
    static constexpr u16  RKCS_GO = (1 << 0);
    static constexpr u16  RKCS_RDY = (1 << 7);
    inline u16 RKCS_FUNCTON (u16 function) { return (function & 7) << 1; }
};


TEST_F (RK11DRegistersTest, controllerIsResponsibleForDefaultAddress)
{
    RK11DConfig rk11dConfig {};

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, nullptr, 
        make_shared<RK11DConfig> (rk11dConfig));

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
    RK11D* rk11dDevice = new RK11D (&bus, nullptr,
        make_shared<RK11DConfig> (rk11dConfig));

    EXPECT_EQ (rk11dDevice->read (BusAddress {RKER}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKCS}), RKCS_RDY);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKWC}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKBA}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {RKDB}), 0);
}