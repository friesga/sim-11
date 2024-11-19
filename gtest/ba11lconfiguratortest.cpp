#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/ba11lconfig/ba11lconfig.h"

#include <fstream>	
#include <gtest/gtest.h>

using std::holds_alternative;

TEST (BA11_LConfiguratorTest, unknownKeyThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-L]\n"
		"cabinet = 0/0\n"
		"unknown_key = 0\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Unknown key in section BA11-L: unknown_key");
	}
	catch (...)
	{
		FAIL ();
	}
}


TEST (BA11_LConfiguratorTest, missingCabinetPositionThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-L]\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Cabinet position not specified in BA11-L section");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (BA11_LConfiguratorTest, incorrectCabinetPositionThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-L]\n"
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

TEST (BA11_LConfiguratorTest, missingCabinetHeightThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-L]\n"
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

TEST (BA11_LConfiguratorTest, cabinetPositionIsCorrect)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-L]\n"
		"cabinet = 10/20\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the BA11-N so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<BA11_LConfig>> (systemConfig[0]));

	// The device's type is BA11_N so the configuration is a BA11_NConfig object
	shared_ptr<BA11_LConfig> ba11_lConfig =
		get<shared_ptr<BA11_LConfig>> (systemConfig[0]);

	EXPECT_EQ (ba11_lConfig->cabinetPosition->cabinetNr, 10);
	EXPECT_EQ (ba11_lConfig->cabinetPosition->height, 20_ru);
}
