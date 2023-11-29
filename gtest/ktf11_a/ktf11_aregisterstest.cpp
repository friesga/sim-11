#include "qbus/qbus.h"
#include "kd/kdf11_a/cpu/kdf11_a_cpu.h"
#include "kd/kdf11_a/ktf11_a/ktf11_a.h"
#include "msv11d/msv11d.h"

#include <gtest/gtest.h>

class KTF11_ARegisters : public ::testing::Test
{
protected:
    static const int SR0 {01777572};
    static const int SR1 {01777574};
    static const int SR2 {01777576};
    static const int KernelPar7 {0172356};
    static const int KernelDspacePDR0 {0172320};

    Qbus qbus;
    KDF11_A_Cpu kdf11_aCpu {&qbus};
    KTF11_A ktf11_a {&qbus, &kdf11_aCpu};
};

// Verify SR0 can be read and written
TEST_F (KTF11_ARegisters, statusRegister0)
{
    // Verify SR0 initially has value 0
    u16 value {0177777};
    EXPECT_EQ (ktf11_a.read (SR0, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);

    // Verify a new value can be written
    EXPECT_EQ (ktf11_a.writeWord (SR0, 0177777), StatusCode::OK);
    EXPECT_EQ (ktf11_a.read (SR0, &value), StatusCode::OK);
    EXPECT_EQ (value, 0177777);
}

// Verify SR1 is a read-only register that reads as zero and writes to the
// register are ignored
TEST_F (KTF11_ARegisters, statusRegister1)
{
    // Verify SR0 initially has value 0
    u16 value {0177777};
    EXPECT_EQ (ktf11_a.read (SR1, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);

    // Verify a new value cannot be written
    EXPECT_EQ (ktf11_a.writeWord (SR1, 0177777), StatusCode::OK);
    EXPECT_EQ (ktf11_a.read (SR1, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);
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
    EXPECT_EQ (ktf11_a.read (KernelPar7, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);

    // Verify a new value can be written
    EXPECT_EQ (ktf11_a.writeWord (KernelPar7, 0177777), StatusCode::OK);
    EXPECT_EQ (ktf11_a.read (KernelPar7, &value), StatusCode::OK);
    EXPECT_EQ (value, 0177777);
}

// Verify Kernel D space PDR 0 is not present
TEST_F (KTF11_ARegisters, kernelDSpaceNotPresent)
{
    Qbus qbus;
    KDF11_A_Cpu kdf11_aCpu {&qbus};
    KTF11_A ktf11_a {&qbus, &kdf11_aCpu};

    u16 value {0177777};
    EXPECT_EQ (ktf11_a.read (KernelDspacePDR0, &value), StatusCode::NonExistingMemory);
}

// Verify SR2 is read-only
TEST_F (KTF11_ARegisters, statusRegister2)
{
    // Verify SR2 initially has value 0
    u16 value {0177777};
    EXPECT_EQ (ktf11_a.read (SR2, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);

    // Verify a new value cannot be written
    EXPECT_EQ (ktf11_a.writeWord (SR2, 0177777), StatusCode::OK);
    EXPECT_EQ (ktf11_a.read (SR2, &value), StatusCode::OK);
    EXPECT_EQ (value, 0);
}