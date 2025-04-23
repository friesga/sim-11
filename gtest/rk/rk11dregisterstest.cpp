#include "rk/rk11d/rk11d.h"
#include "bus/unibus/unibus.h"

#include <gtest/gtest.h>

TEST (RK11DRegistersTest, controllerIsResponsibleForDefaultAddress)
{
    RK11DConfig rk11dConfig {};

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, rk11dConfig);

    // Check that the controller is responsible for its base address
    BusAddress busAddress {0177400};
    EXPECT_TRUE (rk11dDevice->responsible (busAddress));

    // Check that the controller is not responsible for other addresses
    busAddress = BusAddress {0177376};
    EXPECT_FALSE (rk11dDevice->responsible (busAddress));

    busAddress = BusAddress {0x177420};
    EXPECT_FALSE (rk11dDevice->responsible (busAddress));
}

// Verify the registers are correcly initialised
TEST (RK11DRegistersTest, registersInitialised)
{
    RK11DConfig rk11dConfig {};

    Unibus bus;
    RK11D* rk11dDevice = new RK11D (&bus, rk11dConfig);

    EXPECT_EQ (rk11dDevice->read (BusAddress {0177402}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {0177406}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {0177410}), 0);
    EXPECT_EQ (rk11dDevice->read (BusAddress {0177416}), 0);
}