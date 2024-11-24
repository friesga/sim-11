#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/kdf11/kdf11_u/kdf11_uconfig/kdf11_uconfig.h"
#include "configdata/consistencychecker/consistencychecker.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (KDF11_UConfiguratorTest, powerUpModeAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-U]\n"
		"power-up_mode = bootstrap\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	SystemConfig systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KDF11-U so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_UConfig>> (systemConfig[0]));

	// The device's type is KDF11-U so the configuration is a KDF11_UConfig object
	auto kdf11_uConfig = 
		get<shared_ptr<KDF11_UConfig>> (systemConfig[0]);

	EXPECT_EQ (kdf11_uConfig->powerUpMode, KD11Config::PowerUpMode::Bootstrap);
}

TEST (KDF11_UConfiguratorTest, invalidPowerUpModeThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-U]\n"
		"power-up_mode = invalid\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Incorrect KD11 power-up_mode value");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (KDF11_UConfiguratorTest, odtPowerUpModeThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-U]\n"
		"power-up_mode = ODT\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Invalid KDF11-U power-up_mode, must be Vector or Bootstrap");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (KDF11_UConfiguratorTest, kernelHaltModeAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-U]\n"
		"kernel_halt = allow_halt\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KDF11-U so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_UConfig>> (systemConfig[0]));

	// The device's type is KDF11-U so the configuration is a KDF11_UConfig object
	auto kdf11_uConfig =
		get<shared_ptr<KDF11_UConfig>> (systemConfig[0]);

	EXPECT_EQ (kdf11_uConfig->kernelHaltMode, KDF11_UConfig::KernelHaltMode::AllowHalt);
}

TEST (KDF11_UConfiguratorTest, invalidKernelHaltModeThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-U]\n"
		"kernel_halt = invalid\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Incorrect KDF11-U kernel_halt value");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (KDF11_UConfiguratorTest, validBootAddressAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-U]\n"
		"boot_address = 0173000\n";
	stream >> ft;

	IniProcessor iniProcessor;
	iniProcessor.process (ft);

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KDF11-U so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_UConfig>> (systemConfig[0]));

	// The device's type is KDF11-U so the configuration is a KDF11_UConfig
	auto kdf11_uConfig =
		get<shared_ptr<KDF11_UConfig>> (systemConfig[0]);

	EXPECT_EQ (kdf11_uConfig->bootAddress, 0173000);
}

TEST (KDF11_UConfiguratorTest, invalidBootAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-U]\n"
		"boot_address = true";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Incorrect boot address in KDF11-U section specified: true");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (KDF11_UConfiguratorTest, IncorrectBootAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-U]\n"
		"boot_address = 0173010";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "KDF11-U boot address must be either 0165000 or 0173000");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (KDF11_UConfiguratorTest, slusHaveDefaultConfiguration)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-U]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KDF11-U so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_UConfig>> (systemConfig[0]));

	// The device's type is KDF11-U so the configuration is a KDF11_UConfig object
	auto kdf11_uConfig =
		get<shared_ptr<KDF11_UConfig>> (systemConfig[0]);

	SLUConfig* sluConfig = (SLUConfig*)kdf11_uConfig->sluConfig.get ();

	EXPECT_EQ (sluConfig->uartConfig[0].enabled_, true);
	EXPECT_EQ (sluConfig->uartConfig[0].baseAddress_, 0177560);
	EXPECT_EQ (sluConfig->uartConfig[0].baseVector_, 060);
	EXPECT_EQ (sluConfig->uartConfig[0].loopback_, true);

	EXPECT_EQ (sluConfig->uartConfig[1].enabled_, true);
	EXPECT_EQ (sluConfig->uartConfig[1].baseAddress_, 0176500);
	EXPECT_EQ (sluConfig->uartConfig[1].baseVector_, 0300);
	EXPECT_EQ (sluConfig->uartConfig[1].loopback_, true);
}