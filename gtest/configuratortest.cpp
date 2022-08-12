#include "configurator/configprocessor/configprocessor.h"

#include <sstream>	
#include <gtest/gtest.h>

// Minimal tests for the ConfigProcessor class
TEST (ConfigProcessorTest, configProcessed)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[RL]\n"
		"controller = RLV12\n"
		"address = 0174400\n"
		"vector = 0160\n"
		"units = 1\n"

		"[RL.unit0]\n"
		"type = RL01\n"
		"filename = rl01.dsk\n"
		"newfile = true\n"
		"read-only = false\n";
		
	stream >> ft;

	ConfigProcessor configProcessor;
	EXPECT_NO_THROW (configProcessor.process (ft));
}

TEST (ConfigProcessorTest, configProcessorThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[RL]\n"
		"controller = RLV13\n";
		
	stream >> ft;

	ConfigProcessor configProcessor;
	try
	{
		configProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Incorrect RL controller type: RLV13");
	}
	catch (...)
	{
		FAIL();
	}
}