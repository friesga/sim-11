#include "concepts/geometry/geometry.h"

#include <gtest/gtest.h>
#include <stdexcept>

using std::out_of_range;

TEST (GeometryTest, getters)
{
    Geometry geometry (1, 2, 3, 4);
    EXPECT_EQ (geometry.sectorsPerSurface (), 1);
    EXPECT_EQ (geometry.numberOfHeads (), 2);
    EXPECT_EQ (geometry.cylindersPerDisk (), 3);
    EXPECT_EQ (geometry.wordsPerSector (), 4);
}

TEST (GeometryTest, lbnCalculatedCorrectly)
{
    Geometry rk05Geometry_ {12, 2, 203, 256};
    EXPECT_EQ (rk05Geometry_.LBN (DiskAddress {0, 0, 0}), 0);
    EXPECT_EQ (rk05Geometry_.LBN (DiskAddress {11, 0, 0}), 11);
    EXPECT_EQ (rk05Geometry_.LBN (DiskAddress {0, 1, 0}), 12);
    EXPECT_EQ (rk05Geometry_.LBN (DiskAddress {11, 1, 0}), 23);
    EXPECT_EQ (rk05Geometry_.LBN (DiskAddress {0, 0, 1}), 24);
    EXPECT_EQ (rk05Geometry_.LBN (DiskAddress {0, 1, 1}), 36);
    EXPECT_EQ (rk05Geometry_.LBN (DiskAddress {1, 1, 1}), 37);
    EXPECT_EQ (rk05Geometry_.LBN (DiskAddress {0, 0, 202}), 4848);
}

TEST (GeometryTest, diskAddressCalculatedCorrectly)
{
    Geometry rk05Geometry_ {12, 2, 203, 256};

    EXPECT_EQ (rk05Geometry_.lbnTodiskAddress (0),  DiskAddress (0, 0, 0));
    EXPECT_EQ (rk05Geometry_.lbnTodiskAddress (11), DiskAddress (11, 0, 0));
    EXPECT_EQ (rk05Geometry_.lbnTodiskAddress (12), DiskAddress (0, 1, 0));
    EXPECT_EQ (rk05Geometry_.lbnTodiskAddress (23), DiskAddress (11, 1, 0));
    EXPECT_EQ (rk05Geometry_.lbnTodiskAddress (24), DiskAddress (0, 0, 1));
    EXPECT_EQ (rk05Geometry_.lbnTodiskAddress (36), DiskAddress (0, 1, 1));
    EXPECT_EQ (rk05Geometry_.lbnTodiskAddress (37), DiskAddress (1, 1, 1));
    EXPECT_EQ (rk05Geometry_.lbnTodiskAddress (4848), DiskAddress (0, 0, 202));
}

TEST (GeometryTest, incorrectSectorAddressThrows)
{
    Geometry rk05Geometry_ {12, 2, 203, 256};

    EXPECT_THROW (rk05Geometry_.LBN (DiskAddress {12, 0, 0}), out_of_range);
}

TEST (GeometryTest, incorrectHeadThrows)
{
    Geometry rk05Geometry_ {12, 2, 203, 256};

    EXPECT_THROW (rk05Geometry_.LBN (DiskAddress {0, 2, 0}), out_of_range);
}

TEST (GeometryTest, incorrectCylinderThrows)
{
    Geometry rk05Geometry_ {12, 2, 203, 256};

    EXPECT_THROW (rk05Geometry_.LBN (DiskAddress {0, 0, 203}), out_of_range);
}