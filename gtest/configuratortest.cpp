#include "configdata/configprocessor/configprocessor.h"

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

	ConfigData configProcessor;
	EXPECT_NO_THROW (configProcessor.process (ft));
}

TEST (ConfigProcessorTest, configProcessorThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[RL]\n"
		"controller = RLV13\n";
		
	stream >> ft;

	ConfigData configProcessor;
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

#ifdef _WIN32
static const std::string expectedFileNameUnit0 {"\\somefile"};
static const std::string expectedFileNameUnit1 {"G:\\windowsFileName"};
static const std::string expectedFileNameUnit2 {"unqualifiedName"};
static const std::string expectedFileNameUnit3 {"windowsFileName"};
#else
static const std::string expectedFileNameUnit0 {"\\somefile"};
static const std::string expectedFileNameUnit1 {"/mnt/g/sim-11/linuxFileName"};
static const std::string expectedFileNameUnit2 {"linuxFileName"};
static const std::string expectedFileNameUnit3 {"unqualifiedName"};
#endif

TEST (ConfigProcessorTest, fileName)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[RL]\n"
		"units = 4\n"
		"[RL.unit0]\n"
		"filename = \\somefile\n"
		"[RL.unit1]\n"
		"filename = Windows:{G:\\windowsFileName}, Linux:{/mnt/g/sim-11/linuxFileName\n"
		"[RL.unit2]\n"
		"filename = Linux:linuxFileName, unqualifiedName\n"
		"[RL.unit3]\n"
		"filename = Windows:windowsFileName, unqualifiedName\n";
		
	stream >> ft;

	ConfigData configProcessor;
	configProcessor.process (ft);

	EXPECT_STREQ (configProcessor.getConfig()->rlConfig->rlUnitConfig[0].fileName.c_str(),
		"somefile");
	EXPECT_STREQ (configProcessor.getConfig()->rlConfig->rlUnitConfig[1].fileName.c_str(),
		expectedFileNameUnit1.c_str());
	EXPECT_STREQ (configProcessor.getConfig()->rlConfig->rlUnitConfig[2].fileName.c_str(),
		expectedFileNameUnit2.c_str());
	EXPECT_STREQ (configProcessor.getConfig()->rlConfig->rlUnitConfig[3].fileName.c_str(),
		expectedFileNameUnit3.c_str());
}