#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/consistencychecker/consistencychecker.h"

#include <fstream>	
#include <gtest/gtest.h>

#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/consistencychecker/consistencychecker.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (CabinetConsistencyTest, unitOutOfRangeThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"cabinet = 0/20\n";

	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkCabinetConsistency ();
		FAIL ();
	}
	catch (std::out_of_range const& except)
	{
		EXPECT_STREQ (except.what (), "BA11-N cabinet position out of range");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (CabinetConsistencyTest, invalidCabinetThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"cabinet = 1/19\n";

	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkCabinetConsistency ();
		FAIL ();
	}
	catch (std::out_of_range const& except)
	{
		EXPECT_STREQ (except.what (), "BA11-N cabinet position out of range");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (CabinetConsistencyTest, overlappingUnitsThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"cabinet = 0/10\n"
		"[RL]\n"
		"units = 1\n"

		"[RL.unit0]\n"
		"type = RL01\n"
		"cabinet = 0/8\n";

	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkCabinetConsistency ();
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "RL01/02 cabinet position already occupied");
	}
	catch (...)
	{
		FAIL ();
	}
}