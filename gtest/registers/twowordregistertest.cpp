#include "basicregister/twowordregister.h"

#include <gtest/gtest.h>

TEST (TwoWordRegisterTest, twoWordRegisterCanBeAdssigned)
{
    TwoWordRegister twoWordRegister {0};
    twoWordRegister = 0177777;

    EXPECT_EQ (twoWordRegister.low, 0177777);
    EXPECT_EQ (twoWordRegister.high, 0);

    twoWordRegister = 037777600000;

    EXPECT_EQ (twoWordRegister.low, 0);
    EXPECT_EQ (twoWordRegister.high, 0177777);
}

