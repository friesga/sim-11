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

TEST (DL11ConfiguratorTest, defaultsAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[DL11]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the DL11 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<DL11Config> (systemConfig[0]));

	auto dl11Config = get<DL11Config> (systemConfig[0]);

	EXPECT_EQ (dl11Config.baseAddress, 0177560);
	EXPECT_EQ (dl11Config.baseVector, 060);
}

TEST (DL11ConfiguratorTest, unknowKeyThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[DL11]\n"
		"unkown_key = value\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_THROW (iniProcessor.process (ft), std::exception);
}

TEST (DL11ConfiguratorTest, incorrectBaseAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[DL11]\n"
		"address = 0173600\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"DL11 base address must be in range 0174000 - 0177776");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (DL11ConfiguratorTest, invalidBaseAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[DL11]\n"
		"address = 0176502\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"DL11 bus address must be on a eight byte boundary");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (DL11ConfiguratorTest, incorrectVectorAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[DL11]\n"
		"vector = 0400\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"DL11 vector address must be in range 0000 - 0376");
	}
	catch (...)
	{
		FAIL ();
	}
}

