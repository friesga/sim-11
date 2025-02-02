#include "memorysize.h"

#include <gtest/gtest.h>

TEST (MemorySizeTest, compilationSucceeds) 
{
    constexpr memorySize mem1 = 2_KiB;
    static_assert(mem1.wordCapacity () == 1024);
    static_assert(mem1.byteCapacity () == 2048);

    constexpr memorySize mem2 = 1_KiW;
    static_assert(mem2.wordCapacity () == 1024);
    static_assert(mem2.byteCapacity () == 2048);

    constexpr memorySize mem3 = Bytes {16};
    static_assert(mem3.wordCapacity () == 8);
    static_assert(mem3.byteCapacity () == 16);

    // Compilation will fail if the above assertions fail, but test
    // the values anyway to mark this as a succeeding test
    EXPECT_TRUE (mem1.wordCapacity () == 1024);
    EXPECT_TRUE (mem1.byteCapacity () == 2048);
    EXPECT_TRUE (mem2.wordCapacity () == 1024);
    EXPECT_TRUE (mem2.byteCapacity () == 2048);
    EXPECT_TRUE (mem3.wordCapacity () == 8);
    EXPECT_TRUE (mem3.byteCapacity () == 16);
}