#include "conddata/conddata.h"

#include <gtest/gtest.h>
#include <string>

using std::string;

TEST (CondDataTest, defaultConstructorCreatesEmptyObject)
{
    CondData<int> cd0;
    EXPECT_TRUE (!cd0.hasValue ());
}

TEST (CondDataTest, accessingEmptyObjectThrows)
{
    CondData<int> cd0;
    EXPECT_THROW (cd0.value (), string);
}

TEST (CondDataTest, valueOrCanAccessEmptyObject)
{
    CondData<int> cd0;
    EXPECT_TRUE (cd0.valueOr (1) == 1);
}

TEST (CondDataTest, objectCanBeConstructedWithValue)
{
    CondData<int> cd0 {5};
    EXPECT_TRUE (cd0.value () == 5);
    EXPECT_TRUE (cd0.hasValue ());
}

TEST (CondDataTest, objectCanBeConstructedWithStatusCode)
{
    CondData<int> cd0 {StatusCode::NonExistingMemory};
    EXPECT_FALSE (cd0.hasValue ());
    EXPECT_EQ (cd0.statusCode (), StatusCode::NonExistingMemory);
}

TEST (CondDataTest, objectCanBeCopyConstructed)
{
    CondData<int> cd0 {5};
    CondData<int> cd1 {cd0};
    EXPECT_EQ (cd0.value (), cd1.value ());
    EXPECT_EQ (cd0.statusCode (), cd1.statusCode ());
}

TEST (CondDataTest, objectCanBeCopied)
{
    CondData<int> cd0 {5, StatusCode::ArgumentError};
    CondData<int> cd1 {10, StatusCode::Success};
    cd0 = cd1;
    EXPECT_EQ (cd0.value (), 10);
    EXPECT_EQ (cd0.statusCode (), StatusCode::Success);
}

TEST (CondDataTest, objectCanBeAssignedTo)
{
    CondData<int> cd0;
    cd0 = 6;
    EXPECT_TRUE (cd0.value () == 6);
    EXPECT_TRUE (cd0.hasValue ());
}

TEST (CondDataTest, objectCanBeConvertedToNativeType)
{
    CondData<int> cd0 {6};
    int tmp = cd0;
    EXPECT_TRUE (tmp == 6);
    EXPECT_TRUE (cd0 == 6);
}

// This test implicitly verifies that a CondData object can be converted to
// the native type (as checked in the previous test).
TEST (CondDataTest, objectCanBeAddedTo)
{
    CondData<int> cd0 {6};
    cd0 += 4;
    EXPECT_TRUE (cd0 == 10);
}

TEST (CondDataTest, objectCanBeSubtractedFrom)
{
    CondData<int> cd0 {6};
    cd0 -= 2;
    EXPECT_TRUE (cd0 == 4);
}

TEST (CondDataTest, objectCanBeSubtractedFromAndAssignedTo)
{
    CondData<int> cd0 {6};
    cd0 -= 2;
    EXPECT_TRUE (cd0 == 4);
}

TEST (CondDataTest, objectCanBeCopiedToAnotherType)
{
    CondData<short> cd0 {6};
    CondData<int> cd1;
    cd1 = cd0;
    EXPECT_TRUE (cd1.hasValue ());
}


// The following two tests are not verifying CondData functionality.
// In the tests a CondData object is converted to the native type on which
// the operation is performed and then a CondData object is created from
// the resulting integer.
TEST (CondDataTest, objectCanBeMasked)
{
    CondData<int> cd0 = CondData<int>{3} & 0x1;
    EXPECT_TRUE (cd0 == 1);
}

TEST (CondDataTest, objectCanBeShiftedRight)
{
    CondData<int> cd0 = CondData<int> {4} >> 2;
    EXPECT_TRUE (cd0 == 1);
}

TEST (CondDataTest, objectCanBePreIncremented)
{
    CondData<int> cd0 = ++CondData<int> {4};
    EXPECT_TRUE (cd0 == 5);
}

TEST (CondDataTest, objectCanBePreDecremented)
{
    CondData<int> cd0 = --CondData<int> {4};
    EXPECT_TRUE (cd0 == 3);
}

TEST (CondDataTest, objectCanBePostIncremented)
{
    CondData<int> cd0 = CondData<int> {4};
    CondData<int> cd1 = cd0++;
    EXPECT_TRUE (cd1 == 4);
    EXPECT_TRUE (cd0 == 5);
}

TEST (CondDataTest, objectCanBePostDecremented)
{
    CondData<int> cd0 = CondData<int> {4};
    CondData<int> cd1 = cd0--;
    EXPECT_TRUE (cd1 == 4);
    EXPECT_TRUE (cd0 == 3);
}