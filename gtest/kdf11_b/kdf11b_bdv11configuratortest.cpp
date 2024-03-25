#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/kdf11_b/kdf11_bconfig/kdf11_bconfig.h"
#include "configdata/bdv11config/bdv11config.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (KDF11B_BDV11ConfiguratorTest, defaultConfigurationAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.BDV11]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<DeviceConfig> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_BConfig>> (systemConfig[0]));

	// The device's type is KD11 so the configuration is a KD11Config object
	auto kdf11_bConfig = 
		get<shared_ptr<KDF11_BConfig>> (systemConfig[0]);

	BDV11Config* bdv11Config = (BDV11Config*) kdf11_bConfig->bdv11Config.get ();
	
	EXPECT_TRUE (bdv11Config->cpuTests);
	EXPECT_TRUE (bdv11Config->memoryTests);
	EXPECT_FALSE (bdv11Config->decnetBoot);
	EXPECT_TRUE (bdv11Config->consoleDialog);
	EXPECT_EQ (bdv11Config->bootDevice, BDV11Config::BootDevice::RX02);
	EXPECT_EQ (bdv11Config->bootROM, BDV11Config::BootROM::BDV11);
}

TEST (KDF11B_BDV11ConfiguratorTest, bootROMAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.BDV11]\n"
		"boot_rom = kdf11-b";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<DeviceConfig> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_BConfig>> (systemConfig[0]));

	// The device's type is KD11 so the configuration is a KD11Config object
	auto kdf11_bConfig = 
		get<shared_ptr<KDF11_BConfig>> (systemConfig[0]);

	BDV11Config* bdv11Config = (BDV11Config*) kdf11_bConfig->bdv11Config.get ();
	
	EXPECT_EQ (bdv11Config->bootROM, BDV11Config::BootROM::KDF11_BA);
}

TEST (KDF11B_BDV11ConfiguratorTest, optionsAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.BDV11]\n"
		"boot_rom = kdf11-b\n"
		"cpu_tests = false\n"
		"memory_tests = false\n"
		"decnet_boot = true\n"
		"console_dialog = false\n"
		"boot_device = RK05";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<DeviceConfig> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_BConfig>> (systemConfig[0]));

	// The device's type is KD11 so the configuration is a KD11Config object
	auto kdf11_bConfig = 
		get<shared_ptr<KDF11_BConfig>> (systemConfig[0]);

	BDV11Config* bdv11Config = (BDV11Config*) kdf11_bConfig->bdv11Config.get ();
	
	EXPECT_EQ (bdv11Config->bootROM, BDV11Config::BootROM::KDF11_BA);
	EXPECT_FALSE (bdv11Config->cpuTests);
	EXPECT_FALSE (bdv11Config->memoryTests);
	EXPECT_TRUE (bdv11Config->decnetBoot);
	EXPECT_FALSE (bdv11Config->consoleDialog);
	EXPECT_EQ (bdv11Config->bootDevice, BDV11Config::BootDevice::RK05);
}

TEST (KDF11B_BDV11ConfiguratorTest, tu58bootDeviceAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.BDV11]\n"
		"boot_rom = kdf11-b\n"
		"boot_device = TU58";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<DeviceConfig> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_BConfig>> (systemConfig[0]));

	// The device's type is KD11 so the configuration is a KD11Config object
	auto kdf11_bConfig = 
		get<shared_ptr<KDF11_BConfig>> (systemConfig[0]);

	BDV11Config* bdv11Config = (BDV11Config*) kdf11_bConfig->bdv11Config.get ();
	
	EXPECT_EQ (bdv11Config->bootDevice, BDV11Config::BootDevice::TU58);
}