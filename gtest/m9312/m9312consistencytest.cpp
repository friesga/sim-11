#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/systemconfig/systemconfig.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/consistencychecker/consistencychecker.h"
#include "configdata/kt24/kt24config/kt24config.h"

#include <fstream>	
#include <gtest/gtest.h>

// Verify that not both a M9312 and KT24 are configured
TEST (M9312ConsistencyTest, justOneROMDeviceConfigured)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[M9312]\n"
		"[KT24]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkM9312Consistency ();
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"Double diagnostic/boot ROM device specified, specify either M9312 or KT24");
	}
	catch (...)
	{
		FAIL ();
	}
}

// Verify that either a M9312 or a KT24 is configured
TEST (M9312ConsistencyTest, eitherM9312OrKT24Configured)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkM9312Consistency ();
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"No diagnostic/boot ROM specified, specify M9312 or KT24");
	}
	catch (...)
	{
		FAIL ();
	}
}