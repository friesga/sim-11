#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/systemconfig/systemconfig.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/consistencychecker/consistencychecker.h"
#include "configdata/ba11nconfig/ba11nconfig.h"
#include "configdata/ba11lconfig/ba11lconfig.h"

#include <fstream>	
#include <gtest/gtest.h>

// Verify that if in a Unibus system no processor is configured an error
// is reported.
TEST (UnibusConsistencyTest, noProcessorThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[BA11-L]\n"
		"cabinet = 0/19\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};
	try
	{
		consistencyChecker.checkUnibusConsistency ();
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"No processor configured, this system cannot run.");
	}
	catch (...)
	{
		FAIL ();
	}
}