#include "circular_buffer/circular_buffer.h"

#include <gtest/gtest.h>

TEST (CircularBufferTest, BufferCanBeWrittenAndRead)
{
    CircularBuffer<int, 5> testBuffer {};
    ASSERT_TRUE (testBuffer.put (1));
    ASSERT_TRUE (testBuffer.put (2));
    ASSERT_TRUE (testBuffer.put (3));
    ASSERT_TRUE (testBuffer.put (4));
    ASSERT_TRUE (testBuffer.put (5));

    EXPECT_EQ (testBuffer.get (), 1);
    EXPECT_EQ (testBuffer.get (), 2);
    EXPECT_EQ (testBuffer.get (), 3);
    EXPECT_EQ (testBuffer.get (), 4);
    EXPECT_EQ (testBuffer.get (), 5);
}

TEST (CircularBufferTest, BufferDetectedEmpty)
{
    CircularBuffer<int, 5> testBuffer {};

    CondData<int> item = testBuffer.get ();
    ASSERT_FALSE (item.hasValue ());

    ASSERT_TRUE (testBuffer.put (1));

    item = testBuffer.get ();
    ASSERT_TRUE (item.hasValue ());
    ASSERT_EQ (item, 1);
}

TEST (CircularBufferTest, SizeReportedCorrectly)
{
    CircularBuffer<int, 5> testBuffer {};
    ASSERT_EQ (testBuffer.size (), 0);
    ASSERT_TRUE (testBuffer.empty ());

    ASSERT_TRUE (testBuffer.put (1));
    ASSERT_EQ (testBuffer.size (), 1);
}

TEST (CircularBufferTest, OverflowDetected)
{
    CircularBuffer<int, 2> testBuffer {};

    // Verify the buffer can hold exactly two elements and than is full
    ASSERT_TRUE (testBuffer.put (1));
    ASSERT_TRUE (testBuffer.put (2));
    ASSERT_TRUE (testBuffer.full ());
    ASSERT_FALSE (testBuffer.put (3));

    // Verify the contents are not affected by the overflow 
    EXPECT_EQ (testBuffer.get (), 1);
    EXPECT_EQ (testBuffer.get (), 2);

    // Verify the buffer now is empty
    ASSERT_EQ (testBuffer.size (), 0);
    ASSERT_TRUE (testBuffer.empty ());
    CondData<int> item = testBuffer.get ();
    ASSERT_FALSE (item.hasValue ());
}

TEST (CircularBufferTest, OneElement)
{
    CircularBuffer<int, 1> testBuffer {};

    // Verify the buffer can hold one element and than is full
    ASSERT_TRUE (testBuffer.put (1));
    ASSERT_TRUE (testBuffer.full ());
    ASSERT_FALSE (testBuffer.put (2));
    
    EXPECT_EQ (testBuffer.get (), 1);

    // Verify the buffer now is empty
    ASSERT_EQ (testBuffer.size (), 0);
    ASSERT_TRUE (testBuffer.empty ());

    CondData<int> item = testBuffer.get ();
    ASSERT_FALSE (item.hasValue ());
}
