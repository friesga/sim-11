#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/systemconfig/systemconfig.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/consistencychecker/consistencychecker.h"
#include "configdata/ba11/ba11n/ba11nconfig/ba11nconfig.h"
#include "configdata/ba11/ba11l/ba11lconfig/ba11lconfig.h"

#include <fstream>	
#include <gtest/gtest.h>

// Verify that not both a BA11-L and a BA11-N can be configured
TEST (BA11ConsistencyTest, bothBA11_LAndBA11_NCannotBeConfigured)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"cabinet = 0/19\n"
		"[BA11-L]\n"
		"cabinet = 0/16\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkBA11Consistency ();
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"Multiple BA11 specification, specify just one BA11");
	}
	catch (...)
	{
		FAIL ();
	}
}

// Verify that not both a BA11-N and a BA11-L can be configured
TEST (BA11ConsistencyTest, bothBA11_CAndBA11_LCannotBeConfigured)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"cabinet = 0/19\n"
		"[BA11-L]\n"
		"cabinet = 0/16\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkBA11Consistency ();
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"Multiple BA11 specification, specify just one BA11");
	}
	catch (...)
	{
		FAIL ();
	}
}

// Verify that a BA11-N is a Qbus machine and contains no Unibus devices
TEST (BA11ConsistencyTest, noUnibusDevicesOnQbus)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-N]\n"
		"cabinet = 0/19\n"
		"[MS11-P]";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkBA11Consistency ();
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"A Qbus system cannot contain Unibus devices");
	}
	catch (...)
	{
		FAIL ();
	}
}

// Verify that a BA11-L is a Unibus machine and contains no Qbus devices
TEST (BA11ConsistencyTest, noQbusDevicesOnUnibusbus)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-L]\n"
		"cabinet = 0/19\n"
		"[MSV11]";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkBA11Consistency ();
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"A Unibus system cannot contain Qbus devices");
	}
	catch (...)
	{
		FAIL ();
	}
}
