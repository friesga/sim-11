#include <gtest/gtest.h>

#include "kd11/odt/location/location.h"

TEST (LocationTest, AddressLocationCanBeSetAndExamined)
{
    Location testLocation {};

    testLocation = AddressLocation {10};
    testLocation = AddressLocation {20};

    EXPECT_TRUE (testLocation.opened<AddressLocation> ().address () == 20);
    EXPECT_TRUE (testLocation.previous<AddressLocation> ().address () == 10);
}

TEST (LocationTest, RegisterLocationCanBeSetAndExamined)
{
    Location testLocation {};

    testLocation = RegisterLocation {0};
    testLocation = RegisterLocation {1};

    EXPECT_TRUE (testLocation.opened<RegisterLocation> ().registerNr () == 1);
    EXPECT_TRUE (testLocation.previous<RegisterLocation> ().registerNr () == 0);
}

// Verify the type of the openend and the previous location can be checked
TEST (LocationTest, LocationTypeIsCorrect)
{
    Location testLocation {};

    // Verify the location is empty
    EXPECT_TRUE  (testLocation.isA<monostate> ());
    EXPECT_FALSE (testLocation.isA<AddressLocation> ());
    EXPECT_FALSE (testLocation.isA<RegisterLocation> ());
    EXPECT_FALSE (testLocation.isA<PSWLocation> ());

    EXPECT_TRUE  (testLocation.previousIsA<monostate> ());
    EXPECT_FALSE (testLocation.previousIsA<AddressLocation> ());
    EXPECT_FALSE (testLocation.previousIsA<RegisterLocation> ());
    EXPECT_FALSE (testLocation.previousIsA<PSWLocation> ());

    // Set the current location
    testLocation = RegisterLocation {0};
    EXPECT_FALSE (testLocation.isA<monostate> ());
    EXPECT_FALSE (testLocation.isA<AddressLocation> ());
    EXPECT_TRUE  (testLocation.isA<RegisterLocation> ());
    EXPECT_FALSE (testLocation.isA<PSWLocation> ());

    EXPECT_TRUE  (testLocation.previousIsA<monostate> ());
    EXPECT_FALSE (testLocation.previousIsA<AddressLocation> ());
    EXPECT_FALSE (testLocation.previousIsA<RegisterLocation> ());
    EXPECT_FALSE (testLocation.previousIsA<PSWLocation> ());

    // Set current location to an AddressLocation and the previous location
    // then should be the RegisterLocation
    testLocation = AddressLocation {0};
    EXPECT_FALSE (testLocation.isA<monostate> ());
    EXPECT_TRUE  (testLocation.isA<AddressLocation> ());
    EXPECT_FALSE (testLocation.isA<RegisterLocation> ());
    EXPECT_FALSE (testLocation.isA<PSWLocation> ());

    EXPECT_FALSE (testLocation.isA<monostate> ());
    EXPECT_FALSE (testLocation.previousIsA<AddressLocation> ());
    EXPECT_TRUE  (testLocation.previousIsA<RegisterLocation> ());
    EXPECT_FALSE (testLocation.previousIsA<PSWLocation> ());
}

TEST (LocationTest, AddressAndRegisterCanBeAccessed)
{
    Location testLocation {};
    testLocation = AddressLocation {100};
    EXPECT_TRUE  (testLocation.address () == 100);
    EXPECT_THROW (testLocation.registerNr (), std::bad_variant_access);

    testLocation = RegisterLocation {1};
    EXPECT_TRUE  (testLocation.registerNr () == 1);
    EXPECT_THROW (testLocation.address (), std::bad_variant_access);

    EXPECT_TRUE  (testLocation.previousAddress () == 100);
    EXPECT_THROW (testLocation.previousRegisterNr (), std::bad_variant_access);
}