#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/devicetype/devicetype.h"
#include "configdata/kd11config/kd11config.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (KD11_NAConfiguratorTest, powerUpModeAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KD11-NA]\n"
		"power-up_mode = ODT\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::KD11_NA);

	// The device's type is KD11 so the configuration is a KD11Config object
	shared_ptr<KD11_NAConfig> kd11_naConfig = 
		static_pointer_cast<KD11_NAConfig> (systemConfig[0]);

	EXPECT_EQ (kd11_naConfig->powerUpMode, KD11_NAConfig::PowerUpMode::ODT);
}

TEST (KD11_NAConfiguratorTest, invalidPowerUpModeThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KD11-NA]\n"
		"power-up_mode = invalid\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Incorrect KD11-NA power-up_mode value");
	}
	catch (...)
	{
		FAIL();
	}
}