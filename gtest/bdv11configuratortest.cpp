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

	vector<DeviceConfigVariant> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The device's type is BDV11 so the configuration is a BDV11Config object
	shared_ptr<BDV11Config> bdv11Config = 
		get<shared_ptr<BDV11Config>> (systemConfig[0]);

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (bdv11Config->deviceType_, DeviceType::BDV11);

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

	vector<DeviceConfigVariant> systemConfig = 
		iniProcessor.getSystemConfig ();



	// The device's type is BDV11 so the configuration is a BDV11Config object
	shared_ptr<BDV11Config> bdv11Config = 
		get<shared_ptr<BDV11Config>> (systemConfig[0]);

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (bdv11Config->deviceType_, DeviceType::BDV11);

	EXPECT_TRUE (bdv11Config->cpuTests);
	EXPECT_TRUE (bdv11Config->memoryTests);
	EXPECT_FALSE (bdv11Config->decnetBoot);
	EXPECT_TRUE (bdv11Config->consoleDialog);
	EXPECT_EQ (bdv11Config->bootDevice, BDV11Config::BootDevice::RX02);
}

TEST (BDV11ConfiguratorTest, bootROMAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[BDV11]\n"
		"boot_rom = kdf11-b";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<DeviceConfigVariant> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The device's type is BDV11 so the configuration is a BDV11Config object
	shared_ptr<BDV11Config> bdv11Config = 
		get<shared_ptr<BDV11Config>> (systemConfig[0]);

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (bdv11Config->deviceType_, DeviceType::BDV11);

	EXPECT_EQ (bdv11Config->bootROM, BDV11Config::BootROM::KDF11_BA);
}

TEST (BDV11ConfiguratorTest, invalidBootROMThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[BDV11]\n"
		"boot_rom = xxx";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Incorrect BDV11 boot rom: xxx");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (BDV11ConfiguratorTest, tu58BootDeviceThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[BDV11]\n"
		"boot_device = TU58";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(),
			"TU58 as boot device is not supported in BDV11 ROMs");
	}
	catch (...)
	{
		FAIL();
	}
}

