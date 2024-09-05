#include "cabinet/cabinet.h"

#include <gtest/gtest.h>
#include <memory>

using std::make_shared;

TEST (CabinetTest, verifyCabinetIsEmpty)
{
    Cabinet testCabinet ("H9642", 5_ru);
    EXPECT_FALSE (testCabinet.sectionOccupied (make_shared<Cabinet::Position> (0, 4_ru), 1_ru));
    EXPECT_FALSE (testCabinet.sectionOccupied (make_shared<Cabinet::Position> (0, 3_ru), 1_ru));
    EXPECT_FALSE (testCabinet.sectionOccupied (make_shared<Cabinet::Position> (0, 2_ru), 1_ru));
    EXPECT_FALSE (testCabinet.sectionOccupied (make_shared<Cabinet::Position> (0, 1_ru), 1_ru));
    EXPECT_FALSE (testCabinet.sectionOccupied (make_shared<Cabinet::Position> (0, 0_ru), 1_ru));

    EXPECT_FALSE (testCabinet.sectionOccupied (make_shared<Cabinet::Position> (0, 4_ru), 5_ru));
}

// Verify a reference to a section outside the cabinet's dimensions throws
TEST (CabinetTest, referenceOutsideCabinetThrows1)
{
    Cabinet testCabinet ("H9642", 20_ru);

    try
    {
        testCabinet.sectionOccupied (make_shared<Cabinet::Position> (0, 20_ru), 1_ru);
        FAIL ();
    }
    catch (std::invalid_argument const& except)
    {
        EXPECT_STREQ (except.what (), "Reference outside the cabinet");
    }
    catch (...)
    {
        FAIL ();
    }
}

// Verify a unit can be add to an empty cabinet and that section is
// then occupied.
TEST (CabinetTest, unitCanBeAdded)
{
    Cabinet testCabinet ("H9642", 5_ru);

    // Add unit at the top position
    EXPECT_TRUE (testCabinet.addUnit (make_shared<Cabinet::Position> (0, 4_ru), 3_ru));

    // Verify the section is now filled
    EXPECT_TRUE (testCabinet.sectionOccupied (make_shared<Cabinet::Position> (0, 4_ru), 1_ru));
    EXPECT_TRUE (testCabinet.sectionOccupied (make_shared<Cabinet::Position> (0, 3_ru), 1_ru));
    EXPECT_TRUE (testCabinet.sectionOccupied (make_shared<Cabinet::Position> (0, 2_ru), 1_ru));

    // Verify other positions are empty
    EXPECT_FALSE (testCabinet.sectionOccupied (make_shared<Cabinet::Position> (0, 1_ru), 1_ru));
    EXPECT_FALSE (testCabinet.sectionOccupied (make_shared<Cabinet::Position> (0, 0_ru), 1_ru));
}

// Verify a unit cannot be put in an occupied section
TEST (CabinetTest, unitCanNotBePutInOccupiedSection)
{
    Cabinet testCabinet ("H9642", 5_ru);

    // Add unit at the top position
    EXPECT_TRUE (testCabinet.addUnit (make_shared<Cabinet::Position> (0, 4_ru), 3_ru));

    // Verify another unit cannot be added in that position
    EXPECT_FALSE (testCabinet.addUnit (make_shared<Cabinet::Position> (0, 4_ru), 3_ru));
}

// Verify a cabinet can be completely filled
TEST (CabinetTest, unitCanBeFilledCompletely)
{
    Cabinet testCabinet ("H9642", 5_ru);

    // Add unit at the top position and one below
    EXPECT_TRUE (testCabinet.addUnit (make_shared<Cabinet::Position> (0, 4_ru), 3_ru));
    EXPECT_TRUE (testCabinet.addUnit (make_shared<Cabinet::Position> (0, 1_ru), 2_ru));

    EXPECT_TRUE (testCabinet.sectionOccupied (make_shared<Cabinet::Position> (0, 4_ru), 5_ru));
}

