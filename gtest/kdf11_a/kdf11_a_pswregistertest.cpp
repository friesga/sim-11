#include "kd/kdf11_a/cpudata/kdf11_a_psw.h"

#include <gtest/gtest.h>

TEST (KDF11_A_PSWREGISTERTEST, PswCanBeRead)
{
    KDF11_A_PSW psw {0};

    ASSERT_EQ ((u16) psw, 0);
}

TEST (KDF11_A_PSWREGISTERTEST, TraceBitSet)
{
    KDF11_A_PSW psw {020};
    ASSERT_TRUE (psw.traceBitSet ());
}

TEST (KDF11_A_PSWREGISTERTEST, PriorityLevel)
{
    KDF11_A_PSW psw {0};
    psw.setPriorityLevel (7);

    ASSERT_EQ ((u16) psw, 0000340);
    ASSERT_EQ (psw.priorityLevel (), 7);
}

TEST (KDF11_A_PSWREGISTERTEST, CurrentAndPreviousMode)
{
    KDF11_A_PSW psw {0170000};

    ASSERT_EQ (psw.currentMode (), KDF11_A_PSW::Mode::User);
    ASSERT_EQ (psw.previousMode (), KDF11_A_PSW::Mode::User);
}
