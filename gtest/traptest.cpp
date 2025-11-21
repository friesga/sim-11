#include "proc/kd/kdf11/cpudata/kdf11cpudata.h"

#include <gtest/gtest.h>

TEST (TrapTest, trapInitiallyNull)
{
    KDF11CpuData cpuData {};

    EXPECT_EQ (cpuData.trap (), CpuData::TrapCondition::None);
}

TEST (TrapTest, trapCanBeSet)
{
    KDF11CpuData cpuData;

    cpuData.setTrap (CpuData::TrapCondition::BusError);
    EXPECT_EQ (cpuData.trap (), CpuData::TrapCondition::BusError);
}

TEST (TrapTest, trapCanBeCleared)
{
    KDF11CpuData cpuData;

    cpuData.setTrap (CpuData::TrapCondition::BusError);
    cpuData.clearTrap ();
    EXPECT_EQ (cpuData.trap (), CpuData::TrapCondition::None);
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
