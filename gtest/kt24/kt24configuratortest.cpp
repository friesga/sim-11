#include "configdata/kt24/kt24config/kt24config.h"
#include "configdata/iniprocessor/iniprocessor.h"

#include <gtest/gtest.h>

TEST (KT24ConfiguratorTest, kt24ModuleAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KT24]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));
}