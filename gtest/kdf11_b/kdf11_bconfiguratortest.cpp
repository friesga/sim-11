#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/kdf11_b/kdf11_bconfig/kdf11_bconfig.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (KDF11_BConfiguratorTest, powerUpModeAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"power-up_mode = ODT\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<DeviceConfigVariant> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_BConfig>> (systemConfig[0]));

	// The device's type is KD11 so the configuration is a KD11Config object
	auto kdf11_bConfig = 
		get<shared_ptr<KDF11_BConfig>> (systemConfig[0]);

	EXPECT_EQ (kdf11_bConfig->powerUpMode, KD11Config::PowerUpMode::ODT);
}

TEST (KDF11_BConfiguratorTest, invalidPowerUpModeThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
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

TEST (KDF11_BConfiguratorTest, KTF11_AOptionThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"KTF11-A = true";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Unknown key in section KDF11-B: KTF11-A");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (KDF11_BConfiguratorTest, BDV11OptionThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[BDV11]";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Specify either KDF11-B or BDV11");
	}
	catch (...)
	{
		FAIL();
	}
}


TEST (KDF11_BConfiguratorTest, startingAddressThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"starting_address = 0173000\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Unknown key in section KDF11-B: starting_address");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (KDF11_BConfiguratorTest, invalidSubSectionThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.XXX]\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Unknown KDF11-B subsection: XXX");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (KDF11_BConfiguratorTest, doubleSubSectionDefinitionThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.BDV11]\n"
		"[KDF11-B.BDV11]\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Double specification for KDF11-B BDV11");
	}
	catch (...)
	{
		FAIL();
	}
}