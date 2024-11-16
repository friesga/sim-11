#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/consistencychecker/consistencychecker.h"
#include "configdata/ba11nconfig/ba11nconfig.h"
#include "configdata/ba11lconfig/ba11lconfig.h"

#include <fstream>	
#include <gtest/gtest.h>

// Verify that not both a BA11-L and a BA11-N are configured
TEST (BA11ConsistencyTest, justOneBA11Configured)
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

	vector<DeviceConfig> systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkBA11Consistency ();
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"Double BA11 specification, specify either BA11-N or BA11-L");
	}
	catch (...)
	{
		FAIL ();
	}
}


// Verify that either a BA11-L or a BA11-N is configured


// Verify that a BA11-N is a Qbus machine and contains no Unibus devices

// Verify that a BA11-L is a Unibus machine and contains no Qbus devices
