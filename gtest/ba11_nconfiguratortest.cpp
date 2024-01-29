#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/devicetype/devicetype.h"
#include "configdata/ba11nconfig/ba11nconfig.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (BA11_NConfiguratorTest, defaultLogoIsPDP11_03L)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the BA11-N so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::BA11_N);

	// The device's type is BA11_N so the configuration is a BA11_NConfig object
	shared_ptr<BA11_NConfig> dlv11Config = 
		static_pointer_cast<BA11_NConfig> (systemConfig[0]);

	EXPECT_EQ (dlv11Config->logo, BA11_NConfig::Logo::PDP_1103L);
}


TEST (BA11_NConfiguratorTest, logoSelected)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"logo = PDP-11/23\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	vector<shared_ptr<DeviceConfig>> systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the BA11-N so if that's
	// not correct the following tests will fail too.
	ASSERT_EQ (systemConfig[0]->deviceType_, DeviceType::BA11_N);

	// The device's type is BA11_N so the configuration is a BA11_NConfig object
	shared_ptr<BA11_NConfig> dlv11Config = 
		static_pointer_cast<BA11_NConfig> (systemConfig[0]);

	EXPECT_EQ (dlv11Config->logo, BA11_NConfig::Logo::PDP_1123);
}

TEST (BA11_NConfiguratorTest, unknownKeyThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"unknown_key = 0\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Unknown key in section BA11-N: unknown_key");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST (BA11_NConfiguratorTest, unavailableLogoThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"logo = unknown\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Unavailable BA11-N logo selected");
	}
	catch (...)
	{
		FAIL();
	}
}

