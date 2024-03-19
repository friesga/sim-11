#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/devicetype/devicetype.h"
#include "configdata/msv11config/msv11config.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (MSV11ConfiguratorTest, unknownKeyThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MSV11]\n"
		"start_address = 0\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Unknown key in section MSV11: start_address");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (MSV11ConfiguratorTest, startingAddressAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[MSV11]\n"
		"starting_address = 020000\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<DeviceConfigVariant> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The device's type is MSV11 so the configuration is a MSV11Config object
	auto msv11Config = 
		get<shared_ptr<MSV11Config>> (systemConfig[0]);

	// The only device type in this testset is the MSV11 so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (msv11Config->deviceType_, DeviceType::MSV11);

	EXPECT_EQ (msv11Config->startingAddress, 020000);
}

// The maximum value of 32-bit unsigned integer is 4294967295.
TEST (MSV11ConfiguratorTest, invalidStartingAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MSV11]\n"
		"starting_address = 4294967296\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Incorrect address in MSV11 section specified: 4294967296");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (MSV11ConfiguratorTest, exceedingMaximumStartingAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MSV11]\n"
		"starting_address = 01000000\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "MSV11 maximum starting address is 0760000");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (MSV11ConfiguratorTest, startingAddressNotAt4KBoundaryThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MSV11]\n"
		"starting_address = 10000\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "MSV11 address must start at 4K boundary");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (MSV11ConfiguratorTest, multipleMSV11SectionsAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[MSV11]\n"
		"starting_address = 0\n"
		"[MSV11]\n"
		"starting_address = 0200000\n"
		"[MSV11]\n"
		"starting_address = 0400000\n"
		"[MSV11]\n"
		"starting_address = 0600000\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<DeviceConfigVariant> systemConfig = 
		iniProcessor.getSystemConfig ();

	// Verify the vector contains two device configurations
	ASSERT_EQ (systemConfig.size (), 4);

	auto msv11Config0 = 
		get<shared_ptr<MSV11Config>> (systemConfig[0]);
	auto msv11Config1 = 
		get<shared_ptr<MSV11Config>> (systemConfig[0]);

	// The only device types in this testset should be the MSV11's
	ASSERT_EQ (msv11Config0->deviceType_, DeviceType::MSV11);
	ASSERT_EQ (msv11Config1->deviceType_, DeviceType::MSV11);

	// The first section should have starting address 0
	EXPECT_EQ (msv11Config0->startingAddress, 0);

	// And the section section should have starting address 020000
	EXPECT_EQ (msv11Config1->startingAddress, 0200000);
}

TEST (MSV11ConfiguratorTest, maxNrOfCardsExceededThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MSV11]\n"
		"starting_address = 0\n"
		"[MSV11]\n"
		"starting_address = 0200000\n"
		"[MSV11]\n"
		"starting_address = 0400000\n"
		"[MSV11]\n"
		"starting_address = 0600000\n"
		"[MSV11]\n"
		"starting_address = 0\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::out_of_range const &except)
	{
		EXPECT_STREQ (except.what(), "Maximum number of MSV11 cards (4) exceeded");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (MSV11ConfiguratorTest, conflictingAddressesThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MSV11]\n"
		"starting_address = 0\n"
		"[MSV11]\n"
		"starting_address = 020000\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "MSV11 starting address conflict");
	}
	catch (...)
	{
		FAIL();
	}
}