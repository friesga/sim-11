#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/devicetype/devicetype.h"
#include "configdata/kdf11_aconfig/kdf11_aconfig.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (KDF11_AConfiguratorTest, powerUpModeAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-A]\n"
		"power-up_mode = ODT\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::KDF11_A);

	// The device's type is KD11 so the configuration is a KD11Config object
	shared_ptr<KDF11_AConfig> kdf11_aConfig = 
		static_pointer_cast<KDF11_AConfig> (systemConfig[0]);

	EXPECT_EQ (kdf11_aConfig->powerUpMode, KDF11_AConfig::PowerUpMode::ODT);
}

TEST (KDF11_AConfiguratorTest, invalidPowerUpModeThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-A]\n"
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
		EXPECT_STREQ (except.what(), "Incorrect KD11 power-up_mode value");
	}
	catch (...)
	{
		FAIL();
	}
}