#include "bus/qbus/qbus.h"
#include "kd/kdf11/kdf11_a/kdf11_a.h"
#include "configdata/kdf11/kdf11_a/kdf11_aconfig/kdf11_aconfig.h"

#include <gtest/gtest.h>
#include <memory>

using std::make_shared;

class KDF11_ADeviceBus : public ::testing::Test
{
protected:
    static const int PSW {01777776};
    static const int SR0 {01777572};
    static const int SR1 {01777574};
    static const int SR2 {01777576};
    static const int KernelPar7 {0172356};
    static const int KernelDspacePDR0 {0172320};

    Qbus bus_;
    KDF11_A kdf11_a {&bus_, make_shared<KDF11_AConfig> (true)};
};

// Verify SR0 can be read and written
TEST_F (KDF11_ADeviceBus, statusRegister0)
{
    // The KDF11-A has to be started to be able to process the exit event
    kdf11_a.start ();

    // Verify SR0 initially has value 0
    u16 value {0177777};
    EXPECT_EQ (kdf11_a.read (SR0, &value), StatusCode::Success);
    EXPECT_EQ (value, 0);

    // Verify a new value can be written
    EXPECT_EQ (kdf11_a.writeWord (SR0, 0177777), StatusCode::Success);
    EXPECT_EQ (kdf11_a.read (SR0, &value), StatusCode::Success);
    EXPECT_EQ (value, 0177777);
}

// Verify SR1 is a read-only register that reads as zero and writes to the
// register are ignored
TEST_F (KDF11_ADeviceBus, statusRegister1)
{
    // The KDF11-A has to be started to be able to process the exit event
    kdf11_a.start ();

    // Verify SR0 initially has value 0
    u16 value {0177777};
    EXPECT_EQ (kdf11_a.read (SR1, &value), StatusCode::Success);
    EXPECT_EQ (value, 0);

    // Verify a new value cannot be written
    EXPECT_EQ (kdf11_a.writeWord (SR1, 0177777), StatusCode::Success);
    EXPECT_EQ (kdf11_a.read (SR1, &value), StatusCode::Success);
    EXPECT_EQ (value, 0);
}

// Verify an illegal address is detected
TEST_F (KDF11_ADeviceBus, illegalAddress)
{
    // The KDF11-A has to be started to be able to process the exit event
    kdf11_a.start ();

    u16 value {0177777};
    EXPECT_EQ (kdf11_a.read (0176600, &value), StatusCode::NonExistingMemory);
}

// Verify Kernel PAR register 7 can be read and written
TEST_F (KDF11_ADeviceBus, kernelPAR7)
{
    // The KDF11-A has to be started to be able to process the exit event
    kdf11_a.start ();

    // Verify PAR initially has value 0
    u16 value {0177777};
    EXPECT_EQ (kdf11_a.read (KernelPar7, &value), StatusCode::Success);
    EXPECT_EQ (value, 0);

    // Verify a new value can be written
    EXPECT_EQ (kdf11_a.writeWord (KernelPar7, 0177777), StatusCode::Success);
    EXPECT_EQ (kdf11_a.read (KernelPar7, &value), StatusCode::Success);
    EXPECT_EQ (value, 0177777);
}

// Verify Kernel D space PDR 0 is not present
TEST_F (KDF11_ADeviceBus, kernelDSpaceNotPresent)
{
    // The KDF11-A has to be started to be able to process the exit event
    kdf11_a.start ();

    u16 value {0177777};
    EXPECT_EQ (kdf11_a.read (KernelDspacePDR0, &value), StatusCode::NonExistingMemory);
}

// Verify SR2 is read-only
TEST_F (KDF11_ADeviceBus, statusRegister2)
{
    // The KDF11-A has to be started to be able to process the exit event
    kdf11_a.start ();

    // Verify SR2 initially has value 0
    u16 value {0177777};
    EXPECT_EQ (kdf11_a.read (SR2, &value), StatusCode::Success);
    EXPECT_EQ (value, 0);

    // Verify a new value cannot be written
    EXPECT_EQ (kdf11_a.writeWord (SR2, 0177777), StatusCode::Success);
    EXPECT_EQ (kdf11_a.read (SR2, &value), StatusCode::Success);
    EXPECT_EQ (value, 0);
}

// Verify PSW can be read and written without the T-bit being set
TEST_F (KDF11_ADeviceBus, ProcessorStatusWord)
{
    // The KDF11-A has to be started to be able to process the exit event
    kdf11_a.start ();

    // Verify SR0 initially has value 0
    u16 value {0177777};
    EXPECT_EQ (kdf11_a.read (PSW, &value), StatusCode::Success);
    EXPECT_EQ (value, 0);

    // Verify a new value can be written
    EXPECT_EQ (kdf11_a.writeWord (PSW, 0177777), StatusCode::Success);
    EXPECT_EQ (kdf11_a.read (PSW, &value), StatusCode::Success);
    EXPECT_EQ (value, 0177757);
}
