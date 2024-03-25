#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/kd11_naconfig/kd11_naconfig.h"

#include <fstream>
#include <gtest/gtest.h>

using std::get;

TEST (KD11_NAConfiguratorTest, powerUpModeAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KD11-NA]\n"
		"power-up_mode = ODT\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<DeviceConfigVariant> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KD11_NAConfig>> (systemConfig[0]));

	// The configuration is a KD11Config object
	shared_ptr<KD11_NAConfig> kd11_naConfig = 
		get<shared_ptr<KD11_NAConfig>> (systemConfig[0]);

	EXPECT_EQ (kd11_naConfig->powerUpMode, KD11Config::PowerUpMode::ODT);
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
		EXPECT_STREQ (except.what(), "Incorrect KD11 power-up_mode value");
	}
	catch (...)
	{
		FAIL();
	}
}