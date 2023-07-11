#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/devicetype/devicetype.h"
#include "configdata/bdv11config/bdv11config.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (BDV11ConfiguratorTest, optionsAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[BDV11]\n"
		"cpu_tests = true\n"
		"memory_tests = true\n"
		"decnet_boot = true\n"
		"console_dialog = true\n"
		"boot_device = RK05";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::BDV11);

	// The device's type is BDV11 so the configuration is a BDV11Config object
	shared_ptr<BDV11Config> bdv11Config = 
		static_pointer_cast<BDV11Config> (systemConfig[0]);

	EXPECT_TRUE (bdv11Config->cpuTests);
	EXPECT_TRUE (bdv11Config->memoryTests);
	EXPECT_TRUE (bdv11Config->decnetBoot);
	EXPECT_TRUE (bdv11Config->consoleDialog);
	EXPECT_EQ (bdv11Config->bootDevice, BDV11Config::BootDevice::RK05);
}

TEST (BDV11ConfiguratorTest, invalidBootDeviceThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[BDV11]\n"
		"boot_device = XXX";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Incorrect BDV11 boot device: XXX");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (BDV11ConfiguratorTest, defaultsOk)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[BDV11]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::BDV11);

	// The device's type is BDV11 so the configuration is a BDV11Config object
	shared_ptr<BDV11Config> bdv11Config = 
		static_pointer_cast<BDV11Config> (systemConfig[0]);

	EXPECT_TRUE (bdv11Config->cpuTests);
	EXPECT_TRUE (bdv11Config->memoryTests);
	EXPECT_FALSE (bdv11Config->decnetBoot);
	EXPECT_TRUE (bdv11Config->consoleDialog);
	EXPECT_EQ (bdv11Config->bootDevice, BDV11Config::BootDevice::RX02);
}