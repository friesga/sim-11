#include "bus/unibus/unibus.h"
#include "kw11l/kw11l.h"

#include <gtest/gtest.h>

class KW11LTest : public ::testing::Test
{
protected:
    Unibus bus;
    KW11L kw11l {&bus, KW11LConfig {}};
    static const u16 kw11lAddress {0177546};
    static const u16 SR_INTERRUPT_ENABLE {0100};
    static const u16 SR_INTERRUPT_MONITOR {0200};
};

TEST_F (KW11LTest, kw11lAddressIsCorrect)
{
    EXPECT_TRUE (kw11l.responsible (kw11lAddress));
}

// The INIT signal clears D06 and sets D07 to initialize the status register
// for a new operation (EK-KW11L-TM-002, p. 3-3).
TEST_F (KW11LTest, kw11lIsCorrectlyInitialized)
{
    u16 statusRegister = kw11l.read (kw11lAddress);
    EXPECT_TRUE (statusRegister == SR_INTERRUPT_MONITOR);
}

TEST_F (KW11LTest, monitorBitCanBeCleared)
{
    u16 statusRegister = kw11l.read (kw11lAddress);
    EXPECT_TRUE (statusRegister == SR_INTERRUPT_MONITOR);

    kw11l.writeWord (kw11lAddress, 0);
    statusRegister = kw11l.read (kw11lAddress);
    EXPECT_TRUE (statusRegister == 0);
}

TEST_F (KW11LTest, nonInterruptMode)
{
    u16 statusRegister = kw11l.read (kw11lAddress);
    EXPECT_TRUE (statusRegister == SR_INTERRUPT_MONITOR);

    // Clear monitor bit
    kw11l.writeWord (kw11lAddress, 0);
    statusRegister = kw11l.read (kw11lAddress);
    EXPECT_TRUE (statusRegister == 0);

    // And wait till it's set agaian
    while (statusRegister == 0)
    {
        statusRegister = kw11l.read (kw11lAddress);
    }
    EXPECT_TRUE (statusRegister == SR_INTERRUPT_MONITOR);
}