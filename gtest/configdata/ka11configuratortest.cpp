#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/ka11/ka11config/ka11config.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (KA11ConfiguratorTest, KA11SectionProcessed)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KA11]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KA11.
	ASSERT_TRUE (holds_alternative<KA11Config> (systemConfig[0]));

	// The device's type is KA11 so the configuration is a KA11Config object
	auto ka11Config = get<KA11Config> (systemConfig[0]);
}

TEST (KA11ConfiguratorTest, illegalKA11SubsectionThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KA11]\n"
		"[KA11.illegal]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Unexpected KA11-A subsection");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (KA11ConfiguratorTest, KY11_ASubsectionProcessed)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KA11]\n"
		"[KA11.KY11-A]\n"
		"cabinet = 0/0\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KA11.
	ASSERT_TRUE (holds_alternative<KA11Config> (systemConfig[0]));

	// The device's type is KA11 so the configuration is a KA11Config object
	auto ka11Config = get<KA11Config> (systemConfig[0]);

	// The KA11 configuration should contain a KY11-A configuration
	EXPECT_TRUE (ka11Config.ky11_aConfig_);
}

TEST (KA11ConfiguratorTest, unknownKY11_AKeyThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KA11]\n"
		"[KA11.KY11-A]\n"
		"unkown_key = value\n";
	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Unknown key in section KY11-A: unkown_key");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (KA11ConfiguratorTest, KY11_ACabinetPositionAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KA11]\n"
		"[KA11.KY11-A]\n"
		"cabinet = 1/2\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KA11.
	ASSERT_TRUE (holds_alternative<KA11Config> (systemConfig[0]));

	// The device's type is KA11 so the configuration is a KA11Config object
	auto ka11Config = get<KA11Config> (systemConfig[0]);

	// The KA11 configuration should contain a KY11-A configuration
	EXPECT_TRUE (ka11Config.ky11_aConfig_);

	EXPECT_EQ (ka11Config.ky11_aConfig_->cabinetPosition->cabinetNr, 1);
	EXPECT_EQ (ka11Config.ky11_aConfig_->cabinetPosition->height, 2);
}

TEST (KA11ConfiguratorTest, missingSlashInCabinetPositionThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KA11]\n"
		"[KA11.KY11-A]\n"
		"cabinet = 1\n";
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

TEST (KA11ConfiguratorTest, KY11_AIncorrectCabinetPositionThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KA11]\n"
		"[KA11.KY11-A]\n"
		"cabinet = a/b\n";
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

TEST (KA11ConfiguratorTest, missingKY11_ACabinetPositionThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KA11]\n"
		"[KA11.KY11-A]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "KY11-A cabinet position not specified");
	}
	catch (...)
	{
		FAIL ();
	}
}
