#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/ka11/ka11config/ka11config.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (KA11ConfiguratorTest, KA11SectionProcessed)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KA11]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the KA11.
	ASSERT_TRUE (holds_alternative<KA11Config> (systemConfig[0]));

	// The device's type is KA11 so the configuration is a MSV11Config object
	auto ka11Config = get<KA11Config> (systemConfig[0]);
}