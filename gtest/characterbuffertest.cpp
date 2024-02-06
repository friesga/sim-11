#include "characterbuffer/characterbuffer.h"

#include <gtest/gtest.h>

TEST (CharacterBufferTest, BufferCanBeWrittenAndRead)
{
    CharacterBuffer<int> testBuffer {};
    ASSERT_TRUE (testBuffer.put (1));

    EXPECT_EQ (testBuffer.get (), 1);
}

TEST (CharacterBufferTest, BufferDetectedEmpty)
{
    CharacterBuffer<int> testBuffer {};

    CondData<int> item = testBuffer.get ();
    ASSERT_FALSE (item.hasValue ());

    ASSERT_TRUE (testBuffer.put (1));

    item = testBuffer.get ();
    ASSERT_TRUE (item.hasValue ());
    ASSERT_EQ (item, 1);
}

TEST (CharacterBufferTest, SizeReportedCorrectly)
{
    CharacterBuffer<int> testBuffer {};
    ASSERT_EQ (testBuffer.size (), 0);
    ASSERT_TRUE (testBuffer.empty ());

    ASSERT_TRUE (testBuffer.put (1));
    ASSERT_EQ (testBuffer.size (), 1);
}

TEST (CharacterBufferTest, OverflowDetected)
{
    CharacterBuffer<int> testBuffer {};

    ASSERT_TRUE (testBuffer.put (1));
    ASSERT_TRUE (testBuffer.full ());
    ASSERT_FALSE (testBuffer.put (3));

    // Verify the contents are not affected by the overflow 
    EXPECT_EQ (testBuffer.get (), 1);

    // Verify the buffer now is empty
    ASSERT_EQ (testBuffer.size (), 0);
    ASSERT_TRUE (testBuffer.empty ());
    CondData<int> item = testBuffer.get ();
    ASSERT_FALSE (item.hasValue ());
}

