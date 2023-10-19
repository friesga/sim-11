#include "kd/common/conditioncodes/conditioncodes.h"

#include <gtest/gtest.h>

class ConditionCodesTest : public ::testing::Test
{
protected:
    static u16 const N = 010;
    static u16 const Z = 004;
    static u16 const V = 002;
    static u16 const C = 001;
};

//               N  Z  V  C
// PSW           0  0  0  0
// set.mask      1  0  0  1
// affected.mask 1  1  0  1
// result        1  0  0  1
//
TEST_F (ConditionCodesTest, CcCanBeSet)
{
    ConditionCodes newValue {.N = true, .Z = false, .C = 1};
    EXPECT_EQ (newValue.updatedConditionCodes (0), N | C);
}

//               N  Z  V  C
// PSW           1  1  1  1
// set.mask      0  1  0  0
// affected.mask 1  1  1  0
// result        0  1  0  1
//
TEST_F (ConditionCodesTest, CcCanBeCleared)
{
    static u16 const N = 010;
    static u16 const Z = 004;
    static u16 const V = 002;
    static u16 const C = 001;

    ConditionCodes newValue {.N = false, .Z = true, .V = false};
    EXPECT_EQ (newValue.updatedConditionCodes (N | Z | V | C), (Z | C));
}
