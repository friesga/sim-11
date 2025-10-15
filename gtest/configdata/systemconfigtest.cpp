#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/systemconfig/systemconfig.h"

#include <ranges>

using std::holds_alternative;
using std::ranges::find_if;

#include <gtest/gtest.h>

TEST (SystemConfigTest, systemConfigContainsCorrectConfigurations)
{
    SystemConfig systemConfig
    {
        KD11_NAConfig {},
        MSV11Config {}
    };

    // Verify the configuration contains the KD11_NA and MSV11 configurations
    // and does not contain some other configuration.
    bool kd11Found = false;
    bool msv11Found = false;
    bool dlv11jFound = false;

    for (const DeviceConfig& device : systemConfig)
    {
        if (holds_alternative<KD11_NAConfig> (device))
        {
            kd11Found = true;
            continue;
        }

        if (holds_alternative<MSV11Config> (device))
        {
            msv11Found = true;
            continue;
        }

        if (holds_alternative<DLV11JConfig> (device))
        {
            dlv11jFound = true;
            continue;
        }
    }

    ASSERT_TRUE (kd11Found);
    ASSERT_TRUE (msv11Found);
    ASSERT_FALSE (dlv11jFound);
}

TEST (SystemConfigTest, systemConfigCanBeAccessedByIndex)
{
    SystemConfig systemConfig
    {
        KD11_NAConfig {},
        MSV11Config {}
    };

    ASSERT_TRUE (holds_alternative<KD11_NAConfig> (systemConfig[0]));
    ASSERT_TRUE (holds_alternative<MSV11Config> (systemConfig[1]));
}

TEST (SystemConfigTest, systemConfigCanBeExtended)
{
    SystemConfig systemConfig
    {
        KD11_NAConfig {},
        MSV11Config {}
    };

    ASSERT_TRUE (systemConfig.size () == 2);
    systemConfig.addDeviceConfig (MSV11Config {});

    ASSERT_TRUE (systemConfig.size () == 3);
    ASSERT_TRUE (holds_alternative<MSV11Config> (systemConfig[2]));
}

TEST (SystemConfigTest, deviceConfigCanBeFound)
{
    SystemConfig systemConfig
    {
        KD11_NAConfig {},
        MSV11Config {}
    };

    auto isKD11_NA = [] (DeviceConfig device)
        {
            return holds_alternative<KD11_NAConfig> (device);
        };

    ASSERT_TRUE (find_if (systemConfig, isKD11_NA) != systemConfig.end ());
}

TEST (SystemConfigTest, deviceConfigCanBeRemoved)
{
    SystemConfig systemConfig
    {
        KD11_NAConfig {},
        MSV11Config {}
    };

    // Find the MSV11Config object
    auto isKD11_NA = [] (DeviceConfig device)
        {
            return holds_alternative<KD11_NAConfig> (device);
        };

    auto it = find_if (systemConfig, isKD11_NA);

    // Remove the found object
    systemConfig.remove (it);

    // Verify that the object has been removed
    ASSERT_TRUE (find_if (systemConfig, isKD11_NA) == systemConfig.end ());
}

TEST (SystemConfigTest, systemConfigHasCorrectSize)
{
    SystemConfig systemConfig
    {
        KD11_NAConfig {},
        MSV11Config {}
    };

    auto isKD11_NA = [] (DeviceConfig device)
        {
            return holds_alternative<KD11_NAConfig> (device);
        };

    auto isMSV11Config = [] (DeviceConfig device)
        {
            return holds_alternative<MSV11Config> (device);
        };

    // Remove objects from the system configuration while checking the size
    ASSERT_TRUE (systemConfig.size () == 2);
    systemConfig.remove (find_if (systemConfig, isKD11_NA));
    ASSERT_TRUE (systemConfig.size () == 1);
    systemConfig.remove (find_if (systemConfig, isMSV11Config));
    ASSERT_TRUE (systemConfig.size () == 0);
    ASSERT_TRUE (systemConfig.empty ());
}

TEST (SystemConfigTest, iteratorsCanBeCompared)
{
    SystemConfig systemConfig
    {
        KD11_NAConfig {},
        MSV11Config {}
    };

    // Find the MSV11Config object
    auto isKD11_NA = [] (DeviceConfig device)
        {
            return holds_alternative<KD11_NAConfig> (device);
        };

    auto it1 = find_if (systemConfig, isKD11_NA);
    auto it2 = find_if (systemConfig, isKD11_NA);
    ASSERT_TRUE (it1 == it2);
}

TEST (SystemConfigTest, QbusSystemCorrectlyIdentified)
{
    SystemConfig systemConfig
    {
        BA11_NConfig {},
        MSV11Config {}
    };

    ASSERT_TRUE (systemConfig.isQbusSystem ());
}

