#include "kd/kdf11_a/cpu/kdf11_aregisters/kdf11_aregisters.h"

#include <gtest/gtest.h>
#include <string>

using std::string;

class KDF11_ARegistersTest : public ::testing::Test
{
protected:
    enum {KernelMode   = 0000000};
    enum {ReservedMode = 0040000};
    enum {IllegalMode  = 0100000};
    enum {UserMode     = 0140000};

    u16 psw {0};
    KDF11_ARegisters registers {psw};

    void setR6 (u16 memMgmtMode, u16 value)
    {
        psw = memMgmtMode;
        registers [6] = value;
    }

    u16 getR6 (u16 memMgmtMode)
    {
        psw = memMgmtMode;
        return registers [6];
    }
};

// Verify all registers are initialized to zero
TEST_F (KDF11_ARegistersTest, RegistersInitializedToZero)
{
    EXPECT_EQ (registers [0], 0);
    EXPECT_EQ (registers [1], 0);
    EXPECT_EQ (registers [2], 0);
    EXPECT_EQ (registers [3], 0);
    EXPECT_EQ (registers [4], 0);
    EXPECT_EQ (registers [5], 0);
    EXPECT_EQ (registers [6], 0);
    EXPECT_EQ (registers [7], 0);
}

// Verify all stack pointers are initialized to zero
TEST_F (KDF11_ARegistersTest, StackPointersInitializedToZero)
{
    EXPECT_EQ (getR6 (KernelMode), 0);
    EXPECT_EQ (getR6 (ReservedMode), 0);
    EXPECT_EQ (getR6 (IllegalMode), 0);
    EXPECT_EQ (getR6 (UserMode), 0);
}

TEST_F (KDF11_ARegistersTest, RegistersCanBeAssignedTo)
{
    registers [0] = 10;
    EXPECT_EQ (registers [0], 10);
}

TEST_F (KDF11_ARegistersTest, ExceptionOnIllegalRegisterNr)
{
    EXPECT_THROW (registers [8], string);
}

// Verify all stack pointers are independent
TEST_F (KDF11_ARegistersTest, StackPointersAreIndependent)
{
    setR6 (KernelMode, 10);
    setR6 (ReservedMode, 11);
    setR6 (IllegalMode, 12);
    setR6 (UserMode, 13);

    EXPECT_EQ (getR6 (KernelMode), 10);
    EXPECT_EQ (getR6 (ReservedMode), 11);
    EXPECT_EQ (getR6 (IllegalMode), 12);
    EXPECT_EQ (getR6 (UserMode), 13);
}