#include "kd/kd11_na/cpu/kd11_naregisters/kd11_naregisters.h"

#include <gtest/gtest.h>
#include <string>

using std::string;

// Verify all registers are initialized to zero
TEST (KD11_NARegistersTest, RegistersInitializedToZero)
{
    KD11_NARegisters registers;
    EXPECT_EQ (registers [0], 0);
    EXPECT_EQ (registers [1], 0);
    EXPECT_EQ (registers [2], 0);
    EXPECT_EQ (registers [3], 0);
    EXPECT_EQ (registers [4], 0);
    EXPECT_EQ (registers [5], 0);
    EXPECT_EQ (registers [6], 0);
    EXPECT_EQ (registers [7], 0);
}

TEST (KD11_NARegistersTest, RegistersCanBeAssignedTo)
{
    KD11_NARegisters registers;

    registers [0] = 10;
    EXPECT_EQ (registers [0], 10);
}

TEST (KD11_NARegistersTest, ExceptionOnIllegalRegisterNr)
{
    KD11_NARegisters registers;

    EXPECT_THROW (registers [8], string);
}