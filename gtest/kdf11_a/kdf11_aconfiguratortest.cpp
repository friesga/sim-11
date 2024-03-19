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

	vector<DeviceConfigVariant> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The device's type is KD11 so the configuration is a KD11Config object
	auto kdf11_aConfig = 
		get<shared_ptr<KDF11_AConfig>> (systemConfig[0]);

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (kdf11_aConfig->deviceType_, DeviceType::KDF11_A);

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

TEST (KDF11_AConfiguratorTest, KTF11_AisDetected)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-A]\n"
		"KTF11-A = true";
	stream >> ft;

	IniProcessor iniProcessor;
	iniProcessor.process (ft);

	vector<DeviceConfigVariant> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The device's type is DLV11J so the configuration is a 
	auto kdf11_aConfig = 
		get<shared_ptr<KDF11_AConfig>> (systemConfig[0]);

	// The only device type in this testset is the KDF11-A so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (kdf11_aConfig->deviceType_, DeviceType::KDF11_A);

	EXPECT_TRUE (kdf11_aConfig->ktf11_a_present);
}

TEST (KDF11_AConfiguratorTest, KD11AndKDF11_AOptionsAreProcessed)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-A]\n"
		"power-up_mode = ODT\n"
		"KTF11-A = true";
	stream >> ft;

	IniProcessor iniProcessor;
	iniProcessor.process (ft);

	vector<DeviceConfigVariant> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The device's type is DLV11J so the configuration is a 
	auto kdf11_aConfig = 
		get<shared_ptr<KDF11_AConfig>> (systemConfig[0]);

	// The only device type in this testset is the KDF11-A so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (kdf11_aConfig->deviceType_, DeviceType::KDF11_A);

	EXPECT_EQ (kdf11_aConfig->powerUpMode, KDF11_AConfig::PowerUpMode::ODT);
	EXPECT_TRUE (kdf11_aConfig->ktf11_a_present);
}

TEST (KDF11_AConfiguratorTest, unknownOptionThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-A]\n"
		"option = unknown";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Unknown KD11 option: option");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (KDF11_AConfiguratorTest, validStartingAddressAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-A]\n"
		"starting_address = 0173000\n";
	stream >> ft;

	IniProcessor iniProcessor;
	iniProcessor.process (ft);

	vector<DeviceConfigVariant> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The device's type is DLV11J so the configuration is a 
	auto kdf11_aConfig = 
		get<shared_ptr<KDF11_AConfig>> (systemConfig[0]);

	// The only device type in this testset is the KDF11-A so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (kdf11_aConfig->deviceType_, DeviceType::KDF11_A);

	EXPECT_EQ (kdf11_aConfig->startingAddress, 0173000);
}

TEST (KDF11_AConfiguratorTest, invalidStartingAddressThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-A]\n"
		"starting_address = true";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Incorrect starting address in KDF11-A section specified: true");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (KDF11_AConfiguratorTest, startingAddressNotOn256WordBoundaryThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-A]\n"
		"starting_address = 0200";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "KDF11-A starting address must reside on 256-word boundary");
	}
	catch (...)
	{
		FAIL();
	}
}