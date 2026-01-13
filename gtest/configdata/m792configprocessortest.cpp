#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/m792/m792config/m792config.h"
#include <fstream>	
#include <memory>
#include <gtest/gtest.h>

using std::unique_ptr;
using std::make_unique;

TEST (M792ConfiguratorTest, optionAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[M792]\n"
		"option = M792-YB";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	ASSERT_TRUE (holds_alternative<M792Config> (systemConfig[0]));

	auto m792Config = get<M792Config> (systemConfig[0]);

	EXPECT_EQ (m792Config.option, M792Config::Option::M792_YB);
}

TEST (M792ConfiguratorTest, noOptionSpecifiedThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[M792]\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"No option specified in M792 section");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (M792ConfiguratorTest, illegalOptionSpecifiedThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[M792]\n"
		"option = M792-YA";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"Invalid option specified in M792 section");
	}
	catch (...)
	{
		FAIL ();
	}
}

