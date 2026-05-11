#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/mm11_econfig/mm11_econfig.h"
#include "configdata/consistencychecker/consistencychecker.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (MM11EConfiguratorTest, unknownKeyThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MM11-E]\n"
		"start_address = 0\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Unknown key in section MM11-E: start_address");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (MM11EConfiguratorTest, startingAddressAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MM11-E]\n"
		"starting_address = 040000\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the MM11-E so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<MM11EConfig> (systemConfig[0]));

	// The device's type is MM11-E so the configuration is a MM11-E object
	auto mm11eConfig = get<MM11EConfig> (systemConfig[0]);

	EXPECT_EQ (mm11eConfig.startingAddress, 040000);
}

TEST (MM11EConfiguratorTest, memoryExtensionAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MM11-E]\n"
		"memory_extension = true\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the MM11-E so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<MM11EConfig> (systemConfig[0]));

	// The device's type is MM11-E so the configuration is a MM11-E object
	auto mm11eConfig = get<MM11EConfig> (systemConfig[0]);

	EXPECT_EQ (mm11eConfig.startingAddress, 0);
	EXPECT_EQ (mm11eConfig.memorySizeKB, 16384);
}

TEST (MM11EConfiguratorTest, defaultStartingAddressIsZero)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MM11-E]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the MM11-E so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<MM11EConfig> (systemConfig[0]));

	// The device's type is MS11-P so the configuration is a MS11PConfig object
	auto mm11eConfig = get<MM11EConfig> (systemConfig[0]);

	EXPECT_EQ (mm11eConfig.startingAddress, 0000000);
}

// The maximum value of 32-bit unsigned integer is 4294967295.
TEST (MM11EConfiguratorTest, invalidStartingAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MM11-E]\n"
		"starting_address = 4294967296\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Incorrect starting address in MM11-E section specified: 4294967296");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (MM11EConfiguratorTest, exceedingMaximumStartingAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MM11-E]\n"
		"starting_address = 01000000\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "MM11-E maximum starting address is 0777776");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (MM11EConfiguratorTest, startingAddressNotAt8KBoundaryThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MM11-E]\n"
		"starting_address = 04000\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "MM11-E address must start at 4KW boundary");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (MM11EConfiguratorTest, multipleMM11ESectionsAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MM11-E]\n"
		"starting_address = 0\n"
		"[MM11-E]\n"
		"starting_address = 020000\n"
		"[MM11-E]\n"
		"starting_address = 040000\n"
		"[MM11-E]\n"
		"starting_address = 0100000\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// Verify the vector contains four device configurations
	ASSERT_EQ (systemConfig.size (), 4);

	// The only device types in this testset should be the MM11-E's
	ASSERT_TRUE (holds_alternative<MM11EConfig> (systemConfig[0]));
	ASSERT_TRUE (holds_alternative<MM11EConfig> (systemConfig[1]));

	auto mm11eConfig0 = get<MM11EConfig> (systemConfig[0]);
	auto mm11eConfig1 = get<MM11EConfig> (systemConfig[1]);

	// The first section should have starting address 0
	EXPECT_EQ (mm11eConfig0.startingAddress, 0);

	// And the second section should have starting address 020000
	EXPECT_EQ (mm11eConfig1.startingAddress, 020000);
}

TEST (MM11EConfiguratorTest, conflictingAddressesThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MM11-E]\n"
		"starting_address = 0\n"
        "memory_extension = true\n"
		"[MM11-E]\n"
		"starting_address = 020000\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkMM11EConsistency ();
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "MM11-E starting address conflict");
	}
	catch (...)
	{
		FAIL ();
	}
}
