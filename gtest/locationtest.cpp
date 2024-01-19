#include <gtest/gtest.h>

#include "kd/common/odt/location/location.h"

TEST (LocationTest, AddressLocationCanBeSetAndExamined)
{
    Location<u16> testLocation {};

    testLocation = AddressLocation<u16> {10};
    testLocation = AddressLocation<u16> {20};

    EXPECT_TRUE (testLocation.opened<AddressLocation<u16>> ().wordAddress () == 20);
    EXPECT_TRUE (testLocation.previous<AddressLocation<u16>> ().wordAddress () == 10);
}

TEST (LocationTest, ByteAddressLocationCanBeSetAndExamined)
{
    Location<u16> testLocation {};

    testLocation = AddressLocation<u16> {11};
    testLocation = AddressLocation<u16> {21};

    EXPECT_TRUE (testLocation.opened<AddressLocation<u16>> ().inputAddress () == 21);
    EXPECT_TRUE (testLocation.opened<AddressLocation<u16>> ().wordAddress () == 20);
    EXPECT_TRUE (testLocation.previous<AddressLocation<u16>> ().inputAddress () == 11);
    EXPECT_TRUE (testLocation.previous<AddressLocation<u16>> ().wordAddress () == 10);
}


TEST (LocationTest, RegisterLocationCanBeSetAndExamined)
{
    Location<u16> testLocation {};

    testLocation = RegisterLocation {0};
    testLocation = RegisterLocation {1};

    EXPECT_TRUE (testLocation.opened<RegisterLocation> ().registerNr () == 1);
    EXPECT_TRUE (testLocation.previous<RegisterLocation> ().registerNr () == 0);
}

// Verify the type of the openend and the previous location can be checked
TEST (LocationTest, LocationTypeIsCorrect)
{
    Location<u16> testLocation {};

    // Verify the location is empty
    EXPECT_TRUE  (testLocation.isA<monostate> ());
    EXPECT_FALSE (testLocation.isA<AddressLocation<u16>> ());
    EXPECT_FALSE (testLocation.isA<RegisterLocation> ());
    EXPECT_FALSE (testLocation.isA<PSWLocation> ());

    EXPECT_TRUE  (testLocation.previousIsA<monostate> ());
    EXPECT_FALSE (testLocation.previousIsA<AddressLocation<u16>> ());
    EXPECT_FALSE (testLocation.previousIsA<RegisterLocation> ());
    EXPECT_FALSE (testLocation.previousIsA<PSWLocation> ());

    // Set the current location
    testLocation = RegisterLocation {0};
    EXPECT_FALSE (testLocation.isA<monostate> ());
    EXPECT_FALSE (testLocation.isA<AddressLocation<u16>> ());
    EXPECT_TRUE  (testLocation.isA<RegisterLocation> ());
    EXPECT_FALSE (testLocation.isA<PSWLocation> ());

    EXPECT_TRUE  (testLocation.previousIsA<monostate> ());
    EXPECT_FALSE (testLocation.previousIsA<AddressLocation<u16>> ());
    EXPECT_FALSE (testLocation.previousIsA<RegisterLocation> ());
    EXPECT_FALSE (testLocation.previousIsA<PSWLocation> ());

    // Set current location to an AddressLocation and the previous location
    // then should be the RegisterLocation
    testLocation = AddressLocation<u16> {0};
    EXPECT_FALSE (testLocation.isA<monostate> ());
    EXPECT_TRUE  (testLocation.isA<AddressLocation<u16>> ());
    EXPECT_FALSE (testLocation.isA<RegisterLocation> ());
    EXPECT_FALSE (testLocation.isA<PSWLocation> ());

    EXPECT_FALSE (testLocation.isA<monostate> ());
    EXPECT_FALSE (testLocation.previousIsA<AddressLocation<u16>> ());
    EXPECT_TRUE  (testLocation.previousIsA<RegisterLocation> ());
    EXPECT_FALSE (testLocation.previousIsA<PSWLocation> ());
}

TEST (LocationTest, AddressAndRegisterCanBeAccessed)
{
    Location<u16> testLocation {};
    testLocation = AddressLocation<u16> {100};
    EXPECT_TRUE  (testLocation.wordAddress () == 100);
    EXPECT_THROW (testLocation.registerNr (), std::bad_variant_access);

    testLocation = RegisterLocation {1};
    EXPECT_TRUE  (testLocation.registerNr () == 1);
    EXPECT_THROW (testLocation.wordAddress (), std::bad_variant_access);

    EXPECT_TRUE  (testLocation.previousWordAddress () == 100);
    EXPECT_THROW (testLocation.previousRegisterNr (), std::bad_variant_access);
}

TEST (LocationTest, _32BitAddressLocationCanBeSetAndExamined)
{
    Location<u32> testLocation {};

    testLocation = AddressLocation<u32> {017777777};

    EXPECT_EQ (testLocation.opened<AddressLocation<u32>> ().inputAddress (), 017777777);
    EXPECT_EQ (testLocation.opened<AddressLocation<u32>> ().wordAddress (), 017777776);
}