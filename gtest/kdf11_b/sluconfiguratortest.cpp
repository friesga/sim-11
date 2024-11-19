#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/kdf11_b/kdf11_bconfig/kdf11_bconfig.h"
#include "configdata/serialconfig/uartconfig/uartconfig.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (SLUConfiguratorTest, invalidSubsectionThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
				"[KDF11-B.SLU0]\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Unknown KDF11-B subsection: SLU0");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (SLUConfiguratorTest, subsectionDoubleDefinitionThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
			  "[KDF11-B.SLU]\n"
			  "[KDF11-B.SLU]\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Double specification for KDF11-B SLU");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (SLUConfiguratorTest, defaultConfigurationAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.SLU]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	SystemConfig systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_BConfig>> (systemConfig[0]));

	// The device's type is KD11 so the configuration is a KD11Config object
	auto kdf11_bConfig = 
		get<shared_ptr<KDF11_BConfig>> (systemConfig[0]);

	SLUConfig* sluConfig = (SLUConfig*) kdf11_bConfig->sluConfig.get ();
	
	EXPECT_EQ (sluConfig->uartConfig[0].enabled_, true);
	EXPECT_EQ (sluConfig->uartConfig[0].baseAddress_, 0177560);
	EXPECT_EQ (sluConfig->uartConfig[0].baseVector_, 060);
	EXPECT_EQ (sluConfig->uartConfig[0].loopback_, true);

	EXPECT_EQ (sluConfig->uartConfig[1].enabled_, true);
	EXPECT_EQ (sluConfig->uartConfig[1].baseAddress_, 0176500);
	EXPECT_EQ (sluConfig->uartConfig[1].baseVector_, 0300);
	EXPECT_EQ (sluConfig->uartConfig[1].loopback_, true);
}

TEST (SLUConfiguratorTest, slu2AddressAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.SLU]\n"
		"slu2_address = 0176540";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	SystemConfig systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_BConfig>> (systemConfig[0]));

	// The device's type is KD11 so the configuration is a KD11Config object
	auto kdf11_bConfig = 
		get<shared_ptr<KDF11_BConfig>> (systemConfig[0]);

	SLUConfig* sluConfig = (SLUConfig*) kdf11_bConfig->sluConfig.get ();

	EXPECT_EQ (sluConfig->uartConfig[1].baseAddress_, 0176540);
	EXPECT_EQ (sluConfig->uartConfig[1].baseVector_, 0340);
}

TEST (SLUConfiguratorTest, incorrectSLU2AddressRejected)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.SLU]\n"
		"slu2_address = 0176510";
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
			"KDF11-B SLU2 address must be either 0176500 or 0176540");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (SLUConfiguratorTest, sluDisabledAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.SLU]\n"
		"slu1_enabled = false\n"
		"slu2_enabled = false\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	SystemConfig systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_BConfig>> (systemConfig[0]));

	// The device's type is KD11 so the configuration is a KD11Config object
	auto kdf11_bConfig = 
		get<shared_ptr<KDF11_BConfig>> (systemConfig[0]);

	SLUConfig* sluConfig = (SLUConfig*) kdf11_bConfig->sluConfig.get ();
	
	EXPECT_EQ (sluConfig->uartConfig[0].enabled_, false);
	EXPECT_EQ (sluConfig->uartConfig[1].enabled_, false);
}

TEST (SLUConfiguratorTest, breakResponsedAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.SLU]\n"
		"break_response = boot\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	SystemConfig systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_BConfig>> (systemConfig[0]));

	// The device's type is KD11 so the configuration is a KD11Config object
	auto kdf11_bConfig = 
		get<shared_ptr<KDF11_BConfig>> (systemConfig[0]);

	SLUConfig* sluConfig = (SLUConfig*) kdf11_bConfig->sluConfig.get ();
	
	EXPECT_EQ (sluConfig->consoleConfig.breakResponse, ConsoleConfig::BreakResponse::Boot);
}

TEST (SLUConfiguratorTest, invalidBreakResponseRejected)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.SLU]\n"
		"break_response = incorrect\n";
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
			"Incorrect KDF11-B SLU break_response value");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (SLUConfiguratorTest, escBreakKeyAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.SLU]\n"
		"break_key = esc\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	SystemConfig systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_BConfig>> (systemConfig[0]));

	// The device's type is KD11 so the configuration is a KD11Config object
	auto kdf11_bConfig = 
		get<shared_ptr<KDF11_BConfig>> (systemConfig[0]);

	SLUConfig* sluConfig = (SLUConfig*) kdf11_bConfig->sluConfig.get ();
	
	EXPECT_EQ (sluConfig->consoleConfig.breakKey, 27);
}

TEST (SLUConfiguratorTest, upperCaseControlCharAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.SLU]\n"
		"break_key = ^H\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	SystemConfig systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_BConfig>> (systemConfig[0]));

	// The device's type is KD11 so the configuration is a KD11Config object
	auto kdf11_bConfig = 
		get<shared_ptr<KDF11_BConfig>> (systemConfig[0]);

	SLUConfig* sluConfig = (SLUConfig*) kdf11_bConfig->sluConfig.get ();
	
	EXPECT_EQ (sluConfig->consoleConfig.breakKey, 8);
}

TEST (SLUConfiguratorTest, loopbackAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.SLU]\n"
		"loopback = false\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	SystemConfig systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KDF11_BConfig>> (systemConfig[0]));

	// The device's type is KD11 so the configuration is a KD11Config object
	auto kdf11_bConfig = 
		get<shared_ptr<KDF11_BConfig>> (systemConfig[0]);

	SLUConfig* sluConfig = (SLUConfig*) kdf11_bConfig->sluConfig.get ();
	
	EXPECT_EQ (sluConfig->uartConfig[0].loopback_, false);
	EXPECT_EQ (sluConfig->uartConfig[1].loopback_, false);
}
