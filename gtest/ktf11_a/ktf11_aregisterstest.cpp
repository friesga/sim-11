#include "qbus/qbus.h"
#include "kd/kdf11_a/cpu/kdf11_a_cpu.h"
#include "kd/kdf11_a/ktf11_a/ktf11_a.h"
#include "msv11d/msv11d.h"

#include <gtest/gtest.h>

class KTF11_ARegisters : public ::testing::Test
{
protected:
    Qbus qbus;
    KDF11_A_Cpu kdf11_aCpu {&qbus};
    KTF11_A ktf11_a {&qbus, &kdf11_aCpu};
};

// Verify SR0 can be read and written
TEST_F (KTF11_ARegisters, statusRegister0)
{
    // Verify SR0 initially has value 0
    u16 value {0177777};
    EXPECT_EQ (ktf11_a.read (01777572, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);

    // Verify a new value can be written
    EXPECT_EQ (ktf11_a.writeWord (01777572, 0177777), StatusCode::OK);
    EXPECT_EQ (ktf11_a.read (01777572, &value), StatusCode::OK);
    EXPECT_EQ (value, 0177777);
}

// Verify SR1 is a read-only register
TEST_F (KTF11_ARegisters, statusRegister1)
{
    // Verify SR0 initially has value 0
    u16 value {0177777};
    EXPECT_EQ (ktf11_a.read (01777574, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);

    // Verify a new value cannot be written
    EXPECT_EQ (ktf11_a.writeWord (01777574, 0177777), StatusCode::ReadOnly);
}

// Verify an illegal address is detected
TEST_F (KTF11_ARegisters, illegalAddress)
{
    Qbus qbus;
    KDF11_A_Cpu kdf11_aCpu {&qbus};
    KTF11_A ktf11_a {&qbus, &kdf11_aCpu};

    u16 value {0177777};
    EXPECT_EQ (ktf11_a.read (0176600, &value), StatusCode::NonExistingMemory);
}

// Verify Kernel PAR register 7 can be read and written
TEST_F (KTF11_ARegisters, kernelPAR7)
{
    // Verify PAR initially has value 0
    u16 value {0177777};
    EXPECT_EQ (ktf11_a.read (0172356, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);

    // Verify a new value can be written
    EXPECT_EQ (ktf11_a.writeWord (0172356, 0177777), StatusCode::OK);
    EXPECT_EQ (ktf11_a.read (0172356, &value), StatusCode::OK);
    EXPECT_EQ (value, 0177777);
}

// Verify Kernel D space PDR is not present
TEST_F (KTF11_ARegisters, kernelDSpaceNotPresent)
{
    Qbus qbus;
    KDF11_A_Cpu kdf11_aCpu {&qbus};
    KTF11_A ktf11_a {&qbus, &kdf11_aCpu};

    u16 value {0177777};
    EXPECT_EQ (ktf11_a.read (0172320, &value), StatusCode::NonExistingMemory);
}

// Verify SR2 is read-only
TEST_F (KTF11_ARegisters, statusRegister2IsReadOnly)
{
    // Verify SR2 initially has value 0
    u16 value {0177777};
    EXPECT_EQ (ktf11_a.read (01777576, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);

    // Verify a new value cannot be written
    EXPECT_EQ (ktf11_a.writeWord (01777576, 0177777), StatusCode::OK);
    EXPECT_EQ (ktf11_a.read (01777576, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);
}