#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/systemconfig/systemconfig.h"

#include <memory>

using std::shared_ptr;
using std::make_shared;
using std::holds_alternative;

#include <gtest/gtest.h>

TEST (SystemConfigTest, systemConfigTest)
{
    SystemConfig systemConfig
    {
        make_shared<KD11_NAConfig> (),
        make_shared<MSV11Config> ()
    };

    bool kd11Found = false;
    for (const DeviceConfig& device : systemConfig)
    {
        if (holds_alternative<shared_ptr<KD11_NAConfig>> (device))
            kd11Found = true;
    }

    ASSERT_TRUE (kd11Found);
}