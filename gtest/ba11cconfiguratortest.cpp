#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/ba11cconfig/ba11cconfig.h"

#include <fstream>	
#include <gtest/gtest.h>

using std::holds_alternative;

TEST (BA11_CConfiguratorTest, unknownKeyThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-C]\n"
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
		EXPECT_STREQ (except.what (), "Unknown key in section BA11-C: unknown_key");
	}
	catch (...)
	{
		FAIL ();
	}
}


TEST (BA11_CConfiguratorTest, missingCabinetPositionThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-C]\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Cabinet position not specified in BA11-C section");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (BA11_CConfiguratorTest, incorrectCabinetPositionThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-C]\n"
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

TEST (BA11_CConfiguratorTest, missingCabinetHeightThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-C]\n"
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

TEST (BA11_CConfiguratorTest, cabinetPositionIsCorrect)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-C]\n"
		"cabinet = 10/20\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the BA11-N so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<BA11_CConfig>> (systemConfig[0]));

	// The device's type is BA11_C so the configuration is a BA11_NConfig object
	shared_ptr<BA11_CConfig> ba11_cConfig =
		get<shared_ptr<BA11_CConfig>> (systemConfig[0]);

	EXPECT_EQ (ba11_cConfig->cabinetPosition->cabinetNr, 10);
	EXPECT_EQ (ba11_cConfig->cabinetPosition->height, 20_ru);
}
