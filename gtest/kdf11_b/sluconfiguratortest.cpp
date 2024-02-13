#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/devicetype/devicetype.h"
#include "configdata/kdf11_b/kdf11_bconfig/kdf11_bconfig.h"
#include "configdata/serialconfig/uartconfig/uartconfig.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (KDF11_BConfiguratorTest, invalidSubsectionThrows)
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

TEST (KDF11_BConfiguratorTest, subsectionDoubleDefinitionThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
			  "[KDF11-B.SLU1]\n"
			  "[KDF11-B.SLU1]\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Double specification for KDF11-B SLU1");
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
		"[KDF11-B.SLU1]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KD11 so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::KDF11_B);

	// The device's type is KD11 so the configuration is a KD11Config object
	shared_ptr<KDF11_BConfig> kdf11_bConfig = 
		static_pointer_cast<KDF11_BConfig> (systemConfig[0]);

	EXPECT_EQ (kdf11_bConfig->sluConfig.uartConfig[0].baseAddress_, 0177560);
	EXPECT_EQ (kdf11_bConfig->sluConfig.uartConfig[0].baseVector_, 060);
	EXPECT_EQ (kdf11_bConfig->sluConfig.uartConfig[1].baseAddress_, 0176500);
	EXPECT_EQ (kdf11_bConfig->sluConfig.uartConfig[1].baseVector_, 0300);
}