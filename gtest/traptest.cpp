#include "proc/kd/kdf11/cpudata/kdf11cpudata.h"

#include <gtest/gtest.h>

TEST (TrapTest, trapInitiallyNull)
{
    KDF11CpuData cpuData {};

    EXPECT_FALSE (cpuData.trapPending ());
}

TEST (TrapTest, trapCanBeSet)
{
    KDF11CpuData cpuData;

    cpuData.setTrap (CpuData::TrapCondition::BusError);
    EXPECT_TRUE (cpuData.trapPending ());
}

TEST (TrapTest, trapCanBeCleared)
{
    KDF11CpuData cpuData;

    cpuData.setTrap (CpuData::TrapCondition::BusError);
    cpuData.clearTrap ();
    EXPECT_FALSE (cpuData.trapPending ());
}

TEST (TrapTest, trapVectorCanBeRetrieved)
{
    KDF11CpuData cpuData;

    cpuData.setTrap (CpuData::TrapCondition::BusError);
    EXPECT_EQ (cpuData.trapVector (), 004);
}

TEST (TrapTest, vectorForTrapTypeCanBeRetrieved)
{
    KDF11CpuData cpuData;

    EXPECT_EQ (cpuData.trapVector (CpuData::TrapCondition::BusError), 004);
}

TEST (TrapTest, getVectorForClearTrapThrows)
{
    KDF11CpuData cpuData;

    EXPECT_THROW (cpuData.trapVector (), std::bad_optional_access);
}

TEST (TrapTest, pendingTrapReported)
{
    KDF11CpuData cpuData;

    EXPECT_FALSE (cpuData.trapPending ());
    EXPECT_FALSE (cpuData.trapPending (CpuData::TrapCondition::BusError));

    cpuData.setTrap (CpuData::TrapCondition::BusError);

    EXPECT_TRUE (cpuData.trapPending ());
    EXPECT_TRUE (cpuData.trapPending (CpuData::TrapCondition::BusError));
    EXPECT_FALSE (cpuData.trapPending (CpuData::TrapCondition::IllegalInstructionTrap));
}
