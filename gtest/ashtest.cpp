#include "kd/common/instructions/instructions.h"

#include <gtest/gtest.h>
#include <variant>

using std::get;

TEST (ASH, shiftCountCalculatedCorrectly)
{
    EXPECT_EQ (ASH {072040}.getShiftCount (), -32);
    EXPECT_EQ (ASH {072076}.getShiftCount (), -2);
    EXPECT_EQ (ASH {072077}.getShiftCount (), -1);
    EXPECT_EQ (ASH {072000}.getShiftCount (), 0);
    EXPECT_EQ (ASH {072001}.getShiftCount (), 1);
    EXPECT_EQ (ASH {072037}.getShiftCount (), 31);
}