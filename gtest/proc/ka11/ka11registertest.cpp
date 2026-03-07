#include "proc/ka11/ka11cpudata/ka11cpudata.h"

#include <gtest/gtest.h>

TEST (KA11RegistersTest, pswCanBeAccessed)
{
    KA11CpuData ka11CpuData {};

    EXPECT_EQ (ka11CpuData.writeWord (0177776, 0100), StatusCode::Success);

    CondData<u16> psw = ka11CpuData.read (0177776);
    ASSERT_TRUE (psw.hasValue ());

    EXPECT_EQ (psw, 0100);

    // Verify that some other bus address cannot be accessed via the
    // KA11CpuData
    EXPECT_EQ (ka11CpuData.writeWord (0177774, 0),
        StatusCode::NonExistingMemory);
}