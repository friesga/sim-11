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

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the MSV11 so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::MSV11);

	// The device's type is MSV11 so the configuration is a MSV11Config object
	shared_ptr<MSV11Config> msv11Config = 
		static_pointer_cast<MSV11Config> (systemConfig[0]);

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