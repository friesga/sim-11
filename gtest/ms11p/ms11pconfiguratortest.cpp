#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/ms11pconfig/ms11pconfig.h"
#include "configdata/consistencychecker/consistencychecker.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (MS11PConfiguratorTest, unknownKeyThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MS11-P]\n"
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
		EXPECT_STREQ (except.what (), "Unknown key in section MS11-P: start_address");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (MS11PConfiguratorTest, startingAddressAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MS11-P]\n"
		"starting_address = 040000\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the MS11-P so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<MS11PConfig>> (systemConfig[0]));

	// The device's type is MS11-P so the configuration is a MS11PConfig object
	auto ms11pConfig =
		get<shared_ptr<MS11PConfig>> (systemConfig[0]);

	EXPECT_EQ (ms11pConfig->startingAddress, 040000);
}

// The maximum value of 32-bit unsigned integer is 4294967295.
TEST (MS11PConfiguratorTest, invalidStartingAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MS11-P]\n"
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
		EXPECT_STREQ (except.what (), "Incorrect starting address in MS11-P section specified: 4294967296");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (MS11PConfiguratorTest, exceedingMaximumStartingAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MS11-P]\n"
		"starting_address = 0020000000\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "MS11-P maximum starting address is 016000000");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (MS11PConfiguratorTest, startingAddressNotAt8KBoundaryThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MS11-P]\n"
		"starting_address = 020000\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "MS11-P address must start at 8KW boundary");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (MS11PConfiguratorTest, multipleMS11PSectionsAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MS11-P]\n"
		"starting_address = 0\n"
		"[MS11-P]\n"
		"starting_address = 04000000\n"
		"[MS11-P]\n"
		"starting_address = 010000000\n"
		"[MS11-P]\n"
		"starting_address = 014000000\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// Verify the vector contains two device configurations
	ASSERT_EQ (systemConfig.size (), 4);

	// The only device types in this testset should be the MS11-P's
	ASSERT_TRUE (holds_alternative<shared_ptr<MS11PConfig>> (systemConfig[0]));
	ASSERT_TRUE (holds_alternative<shared_ptr<MS11PConfig>> (systemConfig[1]));

	auto ms11pConfig0 =
		get<shared_ptr<MS11PConfig>> (systemConfig[0]);
	auto ms11pConfig1 =
		get<shared_ptr<MS11PConfig>> (systemConfig[1]);

	// The first section should have starting address 0
	EXPECT_EQ (ms11pConfig0->startingAddress, 0);

	// And the second section should have starting address 04000000
	EXPECT_EQ (ms11pConfig1->startingAddress, 04000000);
}


TEST (MS11PConfiguratorTest, maxNrOfCardsExceededThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MS11-P]\n"
		"starting_address = 0\n"
		"[MS11-P]\n"
		"starting_address = 04000000\n"
		"[MS11-P]\n"
		"starting_address = 010000000\n"
		"[MS11-P]\n"
		"starting_address = 014000000\n"
		"[MS11-P]\n"
		"starting_address = 0\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkMS11Consistency<MS11PConfig, 1024 * 1024> ();
		FAIL ();
	}
	catch (std::out_of_range const& except)
	{
		EXPECT_STREQ (except.what (), "Maximum number of MSV11/MS11-P cards (4) exceeded");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (MS11PConfiguratorTest, conflictingAddressesThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MS11-P]\n"
		"starting_address = 0\n"
		"[MS11-P]\n"
		"starting_address = 03000000\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkMS11Consistency<MS11PConfig, 1024 * 1024> ();
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "MSV11/MS11-P starting address conflict");
	}
	catch (...)
	{
		FAIL ();
	}
}