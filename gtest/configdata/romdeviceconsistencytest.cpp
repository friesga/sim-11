#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/systemconfig/systemconfig.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/consistencychecker/consistencychecker.h"
#include "configdata/kt24/kt24config/kt24config.h"

#include <fstream>	
#include <gtest/gtest.h>

// Verify that not both a M9312 and KT24 can be configured
TEST (ROMDeviceConsistencyTest, eitherM9312OrKT24Configured)
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
		consistencyChecker.checkROMDeviceConsistency ();
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"Multiple diagnostic/boot ROM devices specified, specify one of M9312, KT24 or M792");
	}
	catch (...)
	{
		FAIL ();
	}
}

// Verify that not both a M9312 and M792 can be configured
TEST (ROMDeviceConsistencyTest, eitherM9312OrM792Configured)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[M9312]\n"
		"[M792]\n"
		"option = M792-YB\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkROMDeviceConsistency ();
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"Multiple diagnostic/boot ROM devices specified, specify one of M9312, KT24 or M792");
	}
	catch (...)
	{
		FAIL ();
	}
}

// Verify that at least one ROM device is configured
TEST (ROMDeviceConsistencyTest, atLeastOneROMDeviceConfigured)
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
		consistencyChecker.checkROMDeviceConsistency ();
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"No diagnostic/boot ROM specified, specify M9312, KT24 or M792");
	}
	catch (...)
	{
		FAIL ();
	}
}