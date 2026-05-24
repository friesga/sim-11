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