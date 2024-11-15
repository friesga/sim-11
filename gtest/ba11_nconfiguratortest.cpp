#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/ba11nconfig/ba11nconfig.h"

#include <fstream>	
#include <gtest/gtest.h>

using std::holds_alternative;

TEST (BA11_NConfiguratorTest, defaultLogoIsPDP11_03L)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"cabinet = 0/0";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<DeviceConfig> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the BA11-N so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<BA11_NConfig>> (systemConfig[0]));

	// The device's type is BA11_N so the configuration is a BA11_NConfig object
	shared_ptr<BA11_NConfig> ba11_nConfig = 
		get<shared_ptr<BA11_NConfig>> (systemConfig[0]);

	EXPECT_EQ (ba11_nConfig->logo, BA11_NConfig::Logo::PDP_1103L);
}

TEST (BA11_NConfiguratorTest, logoSelected)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"cabinet = 0/0\n"
		"logo = PDP-11/23\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<DeviceConfig> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the BA11-N so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<BA11_NConfig>> (systemConfig[0]));

	// The device's type is BA11_N so the configuration is a BA11_NConfig object
	shared_ptr<BA11_NConfig> ba11_nConfig = 
		get<shared_ptr<BA11_NConfig>> (systemConfig[0]);

	EXPECT_EQ (ba11_nConfig->logo, BA11_NConfig::Logo::PDP_1123);
}

TEST (BA11_NConfiguratorTest, unknownKeyThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"cabinet = 0/0\n"
		"unknown_key = 0\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Unknown key in section BA11-N: unknown_key");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (BA11_NConfiguratorTest, unavailableLogoThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"cabinet = 0/0\n"
		"logo = unknown\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Unavailable BA11-N logo selected");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (BA11_NConfiguratorTest, missingCabinetPositionThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Cabinet position not specified in BA11-N section");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (BA11_NConfiguratorTest, incorrectCabinetPositionThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"cabinet = xxxx\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Invalid BA11 cabinet position");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (BA11_NConfiguratorTest, missingCabinetHeightThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"cabinet = 10\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Invalid BA11 cabinet position");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (BA11_NConfiguratorTest, cabinetPositionIsCorrect)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"cabinet = 10/20\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	vector<DeviceConfig> systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the BA11-N so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<BA11_NConfig>> (systemConfig[0]));

	// The device's type is BA11_N so the configuration is a BA11_NConfig object
	shared_ptr<BA11_NConfig> ba11_nConfig =
		get<shared_ptr<BA11_NConfig>> (systemConfig[0]);

	EXPECT_EQ (ba11_nConfig->cabinetPosition->cabinetNr, 10);
	EXPECT_EQ (ba11_nConfig->cabinetPosition->height, 20_ru);
}
