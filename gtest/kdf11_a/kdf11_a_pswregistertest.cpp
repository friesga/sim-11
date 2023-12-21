#include "kd/common/kdcpudata/kdpsw.h"

#include <gtest/gtest.h>

TEST (KD_PSWREGISTERTEST, PswCanBeRead)
{
    KD_PSW psw {0};

    ASSERT_EQ ((u16) psw, 0);
}

TEST (KD_PSWREGISTERTEST, ConditionCodesCanBeCleared)
{
    KD_PSW psw {0177777};
    psw.clearConditionCodes (017);

    ASSERT_EQ ((u16) psw, 0177760);
}

TEST (KD_PSWREGISTERTEST, ConditionCodesCanBeSet)
{
    KD_PSW psw {0};
    psw.setConditionCodes (017);

    ASSERT_EQ ((u16) psw, 017);
}

TEST (KD_PSWREGISTERTEST, TraceBitSet)
{
    KD_PSW psw {020};
    ASSERT_TRUE (psw.traceBitSet ());
}

TEST (KD_PSWREGISTERTEST, PriorityLevel)
{
    KD_PSW psw {0};
    psw.setPriorityLevel (7);

    ASSERT_EQ ((u16) psw, 0000340);
    ASSERT_EQ (psw.priorityLevel (), 7);
}

TEST (KD_PSWREGISTERTEST, CurrentAndPreviousMode)
{
    KD_PSW psw {0170000};

    ASSERT_EQ (psw.currentMode (), KD_PSW::Mode::User);
    ASSERT_EQ (psw.previousMode (), KD_PSW::Mode::User);
}
