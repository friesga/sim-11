#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/devicetype/devicetype.h"
#include "configdata/serialconfig/dlconfig/dlconfig.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (DLConfiguratorTest, breakResponseAccepted)
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

	// The device's type is DLV11J so the configuration is a DLConfig object
	shared_ptr<DLConfig> dlConfig = 
		static_pointer_cast<DLConfig> (systemConfig[0]);

	EXPECT_EQ (dlConfig->breakResponse, DLConfig::BreakResponse::Boot);
}

TEST (DLConfiguratorTest, escCharAccepted)
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
	shared_ptr<DLConfig> dlConfig = 
		static_pointer_cast<DLConfig> (systemConfig[0]);

	EXPECT_EQ (dlConfig->breakKey, 27);
}

TEST (DLConfiguratorTest, upperCaseControlCharAccepted)
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
	shared_ptr<DLConfig> dlConfig = 
		static_pointer_cast<DLConfig> (systemConfig[0]);

	EXPECT_EQ (dlConfig->breakKey, 8);
}

TEST (DLConfiguratorTest, lowerCaseControlCharAccepted)
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
	shared_ptr<DLConfig> dlConfig = 
		static_pointer_cast<DLConfig> (systemConfig[0]);

	EXPECT_EQ (dlConfig->breakKey, 8);
}

TEST (DLConfiguratorTest, invalidBreakResponseThrows)
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

TEST (DLConfiguratorTest, unknownBreakKeyThrows)
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

TEST (DLConfiguratorTest, invalidBreakKeyThrows)
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

TEST (DLConfiguratorTest, baseAddressAndVectorAccepted)
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
	shared_ptr<DLConfig> dlConfig = 
		static_pointer_cast<DLConfig> (systemConfig[0]);

	EXPECT_EQ (dlConfig->baseAddress, 0176500);
	EXPECT_EQ (dlConfig->baseVector, 300);
}

TEST (DLConfiguratorTest, invalidBaseAddressThrows)
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

TEST (DLConfiguratorTest, consoleSelectionAccepted)
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
	shared_ptr<DLConfig> dlConfig = 
		static_pointer_cast<DLConfig> (systemConfig[0]);

	EXPECT_TRUE (dlConfig->ch3ConsoleEnabled);
}

TEST (DLConfiguratorTest, consoleEnabledinvalidBaseAddressThrows)
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

TEST (DLConfiguratorTest, loopbackOptionAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"loopback = true\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::DLV11_J);

	// The device's type is DLV11J so the configuration is a 
	shared_ptr<DLConfig> dlConfig = 
		static_pointer_cast<DLConfig> (systemConfig[0]);

	EXPECT_TRUE (dlConfig->loopback);
}