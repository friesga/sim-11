#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/serialconfig/dl11config/dl11config.h"

#include <fstream>	
#include <memory>
#include <gtest/gtest.h>

using std::unique_ptr;
using std::make_unique;


TEST (DL11ConfiguratorTest, baseAddressAndVectorAccepted)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DL11]\n"
		"address = 0176500\n"
		"vector = 0300";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	SystemConfig systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the DL11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<DL11Config> (systemConfig[0]));

	auto dl11Config = get<DL11Config> (systemConfig[0]);

	EXPECT_EQ (dl11Config.baseAddress, 0176500);
	EXPECT_EQ (dl11Config.baseVector, 0300);
}

#if 0
TEST (DLV11JConfiguratorTest, invalidBaseAddressThrows)
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



TEST (DLV11JConfiguratorTest, defaultUARTConfigCreated)
{
	DLV11JConfig dlConfig {};

	EXPECT_EQ (dlConfig.baseAddress, 0176500);
	EXPECT_EQ (dlConfig.baseVector, 0300);

	// The configuration should contain four UARTConfig objects with the
	// correct base address and vector
	EXPECT_EQ (dlConfig.uarts.size (), 4);
	EXPECT_EQ (dlConfig.uarts[0].baseAddress_, 0176500);
	EXPECT_EQ (dlConfig.uarts[0].baseVector_, 0300);

	EXPECT_EQ (dlConfig.uarts[1].baseAddress_, 0176510);
	EXPECT_EQ (dlConfig.uarts[1].baseVector_, 0310);

	EXPECT_EQ (dlConfig.uarts[2].baseAddress_, 0176520);
	EXPECT_EQ (dlConfig.uarts[2].baseVector_, 0320);

	EXPECT_EQ (dlConfig.uarts[3].baseAddress_, 0177560);
	EXPECT_EQ (dlConfig.uarts[3].baseVector_, 060);
}

TEST (DLV11JConfiguratorTest, uartConfigCreated)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"ch3_console_enabled = false\n"
		"address = 0176500\n"
		"vector = 0300";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	SystemConfig systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<DLV11JConfig> (systemConfig[0]));

	// The device's type is DLV11J so the configuration is a 
	auto dlConfig = get<DLV11JConfig> (systemConfig[0]);

	EXPECT_EQ (dlConfig.baseAddress, 0176500);
	EXPECT_EQ (dlConfig.baseVector, 0300);

	// The configuration should contain four UARTConfig objects with the
	// correct base address and vector
	EXPECT_EQ (dlConfig.uarts.size (), 4);
	EXPECT_EQ (dlConfig.uarts[0].baseAddress_, 0176500);
	EXPECT_EQ (dlConfig.uarts[0].baseVector_, 0300);

	EXPECT_EQ (dlConfig.uarts[1].baseAddress_, 0176510);
	EXPECT_EQ (dlConfig.uarts[1].baseVector_, 0310);

	EXPECT_EQ (dlConfig.uarts[2].baseAddress_, 0176520);
	EXPECT_EQ (dlConfig.uarts[2].baseVector_, 0320);

	EXPECT_EQ (dlConfig.uarts[3].baseAddress_, 0176530);
	EXPECT_EQ (dlConfig.uarts[3].baseVector_, 0330);
}

TEST (DLV11JConfiguratorTest, consoleUARTConfigCreated)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"ch3_console_enabled = true\n"
		"address = 0176500\n"
		"vector = 0300";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	SystemConfig systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<DLV11JConfig> (systemConfig[0]));

	// The device's type is DLV11J so the configuration is a 
	auto dlConfig = get<DLV11JConfig> (systemConfig[0]);

	EXPECT_EQ (dlConfig.baseAddress, 0176500);
	EXPECT_EQ (dlConfig.baseVector, 0300);

	// The configuration should contain four UARTConfig objects with the
	// correct base address and vector
	EXPECT_EQ (dlConfig.uarts.size (), 4);
	EXPECT_EQ (dlConfig.uarts[0].baseAddress_, 0176500);
	EXPECT_EQ (dlConfig.uarts[0].baseVector_, 0300);

	EXPECT_EQ (dlConfig.uarts[1].baseAddress_, 0176510);
	EXPECT_EQ (dlConfig.uarts[1].baseVector_, 0310);

	EXPECT_EQ (dlConfig.uarts[2].baseAddress_, 0176520);
	EXPECT_EQ (dlConfig.uarts[2].baseVector_, 0320);

	EXPECT_EQ (dlConfig.uarts[3].baseAddress_, 0177560);
	EXPECT_EQ (dlConfig.uarts[3].baseVector_, 060);
}

TEST (DLV11JConfiguratorTest, alternativeBaseAddressConfigCreated)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[DLV11-J]\n"
		"ch3_console_enabled = false\n"
		"address = 0176540\n"
		"vector = 0200";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft)); 

	SystemConfig systemConfig = 
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the DLV11-J so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<DLV11JConfig> (systemConfig[0]));

	// The device's type is DLV11J so the configuration is a 
	auto dlConfig = get<DLV11JConfig> (systemConfig[0]);

	EXPECT_EQ (dlConfig.baseAddress, 0176540);
	EXPECT_EQ (dlConfig.baseVector, 0200);

	// The configuration should contain four UARTConfig objects with the
	// correct base address and vector
	EXPECT_EQ (dlConfig.uarts.size (), 4);
	EXPECT_EQ (dlConfig.uarts[0].baseAddress_, 0176540);
	EXPECT_EQ (dlConfig.uarts[0].baseVector_, 0200);

	EXPECT_EQ (dlConfig.uarts[1].baseAddress_, 0176550);
	EXPECT_EQ (dlConfig.uarts[1].baseVector_, 0210);

	EXPECT_EQ (dlConfig.uarts[2].baseAddress_, 0176560);
	EXPECT_EQ (dlConfig.uarts[2].baseVector_, 0220);

	EXPECT_EQ (dlConfig.uarts[3].baseAddress_, 0176570);
	EXPECT_EQ (dlConfig.uarts[3].baseVector_, 0230);
}
#endif