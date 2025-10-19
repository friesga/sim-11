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
		"[KA11.KY11-A]\n";
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