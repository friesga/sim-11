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
    BusAddress address {0200000, BusAddress::Width::_18Bit};
    address &= 0xFFFFFFFE;
    ASSERT_EQ ((u32) address, 0200000);
}

// Verify the I/O-page is placed at the correct addresses and that an address
// greater than the maximum value possible for the number of bits wraps to
// zero.
TEST (BusAddressTest, _16bitIOPageIsCorrect)
{
    BusAddress address0 {0157776};
    EXPECT_FALSE (address0.isInIOpage ());

    BusAddress address1 {0160000};
    EXPECT_TRUE (address1.isInIOpage ());
    EXPECT_FALSE (address1.isInIOpageLower2K ());

    BusAddress address2 (0160000 + 4096);
    EXPECT_TRUE (address2.isInIOpage ());
    EXPECT_TRUE (address2.isInIOpageLower2K ());

    BusAddress address3 (0160000 + 8192);
    EXPECT_EQ (static_cast<u16> (address3), 0);
}

TEST (BusAddressTest, _18bitIOPageIsCorrect)
{
    BusAddress address0 {0757776, BusAddress::Width::_18Bit};
    EXPECT_FALSE (address0.isInIOpage ());

    BusAddress address1 {0760000, BusAddress::Width::_18Bit};
    EXPECT_TRUE (address1.isInIOpage ());
    EXPECT_FALSE (address1.isInIOpageLower2K ());

    BusAddress address2 (0760000 + 4096, BusAddress::Width::_18Bit);
    EXPECT_TRUE (address2.isInIOpage ());
    EXPECT_TRUE (address2.isInIOpageLower2K ());

    BusAddress address3 (0760000 + 8192, BusAddress::Width::_18Bit);
    EXPECT_EQ (static_cast<u16> (address3), 0);
}

TEST (BusAddressTest, _22bitIOPageIsCorrect)
{
    BusAddress address0 {017757776, BusAddress::Width::_22Bit};
    EXPECT_FALSE (address0.isInIOpage ());

    BusAddress address1 {017760000, BusAddress::Width::_22Bit};
    EXPECT_TRUE (address1.isInIOpage ());
    EXPECT_FALSE (address1.isInIOpageLower2K ());

    BusAddress address2 (017760000 + 4096, BusAddress::Width::_22Bit);
    EXPECT_TRUE (address2.isInIOpage ());
    EXPECT_TRUE (address2.isInIOpageLower2K ());

    BusAddress address3 (017760000 + 8192, BusAddress::Width::_22Bit);
    EXPECT_EQ (static_cast<u16> (address3), 0);
}
