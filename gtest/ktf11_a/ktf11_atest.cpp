#include "pdp11/pdp11.h"
#include "kd/kdf11/cpudata/kdf11cpudata.h"
#include "kd/kdf11/ktf11_a/ktf11_a.h"

#include <gtest/gtest.h>
#include <functional>

using std::bind;
using std::placeholders::_1;

// Definition of a test fixture
class KTF11_ATest : public testing::Test
{
public:
    bool ioMapEnabled {false};

    void ioMapEnableReceiver (bool signalValue)
    {
        ioMapEnabled = signalValue;
    }
};

// Verify that setting and clearing SR3 bit 5 (Enable I/O mapping) sets and
// clears the IOMapEnable signal.
TEST_F (KTF11_ATest, ioMapCanBeEnabled)
{
    Qbus bus;
    KDF11CpuData cpuData;
    KTF11_A ktf11_a (&bus, &cpuData);

    bus.IOMapEnable ().subscribe (bind (&KTF11_ATest::ioMapEnableReceiver, this, _1));

    ktf11_a.writeWord (0172516, 040);
    EXPECT_TRUE (ioMapEnabled);

    ktf11_a.writeWord (0172516, 0);
    EXPECT_FALSE (ioMapEnabled);
}