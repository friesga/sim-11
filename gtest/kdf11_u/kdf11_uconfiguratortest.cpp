#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/kdf11/kdf11_u/kdf11_uconfig/kdf11_uconfig.h"
#include "configdata/consistencychecker/consistencychecker.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (KDF11_UConfiguratorTest, powerUpModeAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-U]\n"
		"power-up_mode = bootstrap\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	SystemConfig systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KDF11-U so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_UConfig>> (systemConfig[0]));

	// The device's type is KDF11-U so the configuration is a KDF11_UConfig object
	auto kdf11_uConfig = 
		get<shared_ptr<KDF11_UConfig>> (systemConfig[0]);

	EXPECT_EQ (kdf11_uConfig->powerUpMode, KDF11_UConfig::PowerUpMode::Boot);
}

/*
TEST (KDF11_UConfiguratorTest, invalidPowerUpModeThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-U]\n"
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

TEST (KDF11_UConfiguratorTest, KTF11_AOptionThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-U]\n"
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
		EXPECT_STREQ (except.what(), "Unknown key in section KDF11-U: KTF11-A");
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
	stream << "[KDF11-U]\n"
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
		EXPECT_STREQ (except.what(), "Unknown key in section KDF11-U: starting_address");
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
*/