#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/devicetype/devicetype.h"
#include "configdata/dlv11config/dlv11config.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (DLV11ConfiguratorTest, breakResponseAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"ch3_break_response = boot\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::DLV11_J);

	// The device's type is DLV11J so the configuration is a DLV11Config object
	shared_ptr<DLV11Config> dlv11Config = 
		static_pointer_cast<DLV11Config> (systemConfig[0]);

	EXPECT_EQ (dlv11Config->ch3BreakResponse, DLV11Config::Ch3BreakResponse::Boot);
}

TEST (DLV11ConfiguratorTest, escCharAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"break_key = esc\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::DLV11_J);

	// The device's type is DLV11J so the configuration is a 
	shared_ptr<DLV11Config> dlv11Config = 
		static_pointer_cast<DLV11Config> (systemConfig[0]);

	EXPECT_EQ (dlv11Config->breakKey, 27);
}

TEST (DLV11ConfiguratorTest, upperCaseControlCharAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"break_key = ^H\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::DLV11_J);

	// The device's type is DLV11J so the configuration is a 
	shared_ptr<DLV11Config> dlv11Config = 
		static_pointer_cast<DLV11Config> (systemConfig[0]);

	EXPECT_EQ (dlv11Config->breakKey, 8);
}

TEST (DLV11ConfiguratorTest, lowerCaseControlCharAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"break_key = ^h\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::DLV11_J);

	// The device's type is DLV11J so the configuration is a 
	shared_ptr<DLV11Config> dlv11Config = 
		static_pointer_cast<DLV11Config> (systemConfig[0]);

	EXPECT_EQ (dlv11Config->breakKey, 8);
}

TEST (DLV11ConfiguratorTest, invalidBreakResponseThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"ch3_break_response = invalid\n"
		"break_key = esc\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Incorrect ch3_break_response value");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (DLV11ConfiguratorTest, unknownBreakKeyThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"break_key = ab\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Incorrect break key specification");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (DLV11ConfiguratorTest, invalidBreakKeyThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"break_key = ^HA\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Incorrect break key specification");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (DLV11ConfiguratorTest, baseAddressAndVectorAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"address = 0176500\n"
		"vector = 300";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::DLV11_J);

	// The device's type is DLV11J so the configuration is a 
	shared_ptr<DLV11Config> dlv11Config = 
		static_pointer_cast<DLV11Config> (systemConfig[0]);

	EXPECT_EQ (dlv11Config->baseAddress, 0176500);
	EXPECT_EQ (dlv11Config->vector, 300);
}

TEST (DLV11ConfiguratorTest, invalidBaseAddressThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"address = 0150000\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "DLV11-J base address must be in range 0160000 - 0177770");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (DLV11ConfiguratorTest, consoleSelectionAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"ch3_console_enabled = true\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::DLV11_J);

	// The device's type is DLV11J so the configuration is a 
	shared_ptr<DLV11Config> dlv11Config = 
		static_pointer_cast<DLV11Config> (systemConfig[0]);

	EXPECT_TRUE (dlv11Config->ch3ConsoleEnabled);
}

TEST (DLV11ConfiguratorTest, consoleEnabledinvalidBaseAddressThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"ch3_console_enabled = true\n"
		"address = 0176520\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "DLV11-J base address must be 0176500, 0176540 or 177500");
	}
	catch (...)
	{
		FAIL();
	}
}