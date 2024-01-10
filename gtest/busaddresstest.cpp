#include "busaddress/busaddress.h"

#include <gtest/gtest.h>

TEST (BusAddressTest, ConversionToU16Succeeds)
{
    BusAddress address {100};
    ASSERT_EQ ((u16) address, 100);
}

TEST (BusAddressTest, U16CanBeAssigned)
{
    BusAddress address = 100;
    ASSERT_EQ ((u16) address, 100);
}

TEST (BusAddressTest, BusAddressCanBeMasked)
{
    BusAddress address = 0177777;
    address &= 0177776;
    ASSERT_EQ ((u16) address, 0177776);
}

TEST (BusAddressTest, _18BitBusAddressCanBeMasked)
{
    BusAddress address = 0200000;
    address &= 0xFFFFFFFE;
    ASSERT_EQ ((u32) address, 0200000);
}
