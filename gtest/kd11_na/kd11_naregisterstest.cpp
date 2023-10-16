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

TEST (KD11_NARegistersTest, RegistersCanBeConvertedToArray)
{
    KD11_NARegisters registers;
    registers [0] = 10;
    registers [1] = 11;
    registers [2] = 12;
    registers [3] = 13;
    registers [4] = 14;
    registers [5] = 15;
    registers [6] = 16;
    registers [7] = 17;

    u16* arrayPtr = registers;
    EXPECT_EQ (arrayPtr [0], 10);
    EXPECT_EQ (arrayPtr [1], 11);
    EXPECT_EQ (arrayPtr [2], 12);
    EXPECT_EQ (arrayPtr [3], 13);
    EXPECT_EQ (arrayPtr [4], 14);
    EXPECT_EQ (arrayPtr [5], 15);
    EXPECT_EQ (arrayPtr [6], 16);
    EXPECT_EQ (arrayPtr [7], 17);
}