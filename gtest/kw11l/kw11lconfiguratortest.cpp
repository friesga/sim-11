#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/kw11l/kw11lconfig/kw11lconfig.h"
#include "configdata/consistencychecker/consistencychecker.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (KW11LConfiguratorTest, kw11lAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KW11-L]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KW11-L so if that's
	// the test fails
	EXPECT_TRUE (holds_alternative<KW11LConfig> (systemConfig[0]));
}

TEST (KW11LConfiguratorTest, simulatorClockSourceAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KW11-L]\n"
		<< "clock_source=simulator_clock\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KW11-L so if that's
	// the test fails
	EXPECT_TRUE (holds_alternative<KW11LConfig> (systemConfig[0]));

	auto kw11lConfig = get<KW11LConfig> (systemConfig[0]);

	EXPECT_EQ (kw11lConfig.clockSource, KW11LConfig::ClockSource::SimulatorClock);
}

TEST (KW11LConfiguratorTest, systemClockSourceAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KW11-L]\n"
		<< "clock_source=system_clock\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KW11-L so if that's
	// the test fails
	EXPECT_TRUE (holds_alternative<KW11LConfig> (systemConfig[0]));

	auto kw11lConfig = get<KW11LConfig> (systemConfig[0]);

	EXPECT_EQ (kw11lConfig.clockSource, KW11LConfig::ClockSource::SystemClock);
}

TEST (KW11LConfiguratorTest, unknownKeyThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KW11-L]\n"
		"unknown_key = 0\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Unknown key in section KW11-L: unknown_key");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (KW11LConfiguratorTest, incorrectClockSourceThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KW11-L]\n"
		"clock_source=unknown_clock\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Incorrect KW11-L clock source: unknown_clock");
	}
	catch (...)
	{
		FAIL ();
	}
}
