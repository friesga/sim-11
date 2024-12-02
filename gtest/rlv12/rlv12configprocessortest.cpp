#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/rl/rlconfig/rlconfig.h"

#include <fstream>	
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <variant>

using std::shared_ptr;
using std::static_pointer_cast;
using std::vector;
using std::string;
using std::get;

TEST (RLV12ConfigProcessorTest, configProcessed)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RL]\n"
		"controller = RLV12\n"
		"address = 0174400\n"
		"vector = 0160\n"
		"units = 1\n"

		"[RL.unit0]\n"
		"type = RL01\n"
		"cabinet = 0/0\n"
		"filename = rl01.dsk\n"
		"newfile = true\n"
		"write-protect = false\n";
		
	stream >> ft;

	IniProcessor iniProcessor;

	// Verify the configuration is processed without errors
	EXPECT_NO_THROW (iniProcessor.process (ft));

	// Verify the device is present in the configuration with all attributes
	// having their correct value.
	SystemConfig &configuration = 
		iniProcessor.getSystemConfig ();

	ASSERT_TRUE (holds_alternative<shared_ptr<RLV12Config>> (configuration[0]));

	shared_ptr<RLV12Config> rlConfig = 
		get<shared_ptr<RLV12Config>> (configuration[0]);

	EXPECT_EQ (rlConfig->rlType, RLV12Config::RLType::RLV12);
	EXPECT_EQ (rlConfig->address, 0174400);
	EXPECT_EQ (rlConfig->vector, 0160);
	EXPECT_EQ (rlConfig->numUnits, 1);

	EXPECT_EQ (static_pointer_cast<RLUnitConfig>
		(rlConfig->rlUnitConfig[0])->rlUnitType, RLUnitConfig::RLUnitType::RL01);
	EXPECT_EQ (static_pointer_cast<RLUnitConfig>
		(rlConfig->rlUnitConfig[0])->writeProtect, false);
}

TEST (RLV12ConfigProcessorTest, configProcessorThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[RL]\n"
		"controller = RLV13\n";
		
	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
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

TEST (RLV12ConfigProcessorTest, fileName)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[RL]\n"
		"units = 4\n"
		"[RL.unit0]\n"
		"cabinet = 0/0\n"
		"filename = \\somefile\n"
		"[RL.unit1]\n"
		"cabinet = 0/0\n"
		"filename = Windows:{G:\\windowsFileName}, Linux:{/mnt/g/sim-11/linuxFileName\n"
		"[RL.unit2]\n"
		"cabinet = 0/0\n"
		"filename = Linux:linuxFileName, unqualifiedName\n"
		"[RL.unit3]\n"
		"cabinet = 0/0\n"
		"filename = Windows:windowsFileName, unqualifiedName\n";
		
	stream >> ft;

	IniProcessor iniProcessor;
	iniProcessor.process (ft);

	for (auto device : iniProcessor.getSystemConfig ())
    {
		// The only device type in this testset is the RLV12 so if that's
		// not corrected the following tests will fail too.
		ASSERT_TRUE (holds_alternative<shared_ptr<RLV12Config>> (device));

		// The device's type is RLV12 so the configuration is a RLV12Config
		auto rlConfig = 
			get<shared_ptr<RLV12Config>> (device);

		// Now we can check the unit's filenames. The devices in the 
		// units are of type RLUnitConfig.
		EXPECT_STREQ (static_pointer_cast<RLUnitConfig> 
			(rlConfig->rlUnitConfig[0])->fileName.c_str(), "somefile");
		EXPECT_STREQ (static_pointer_cast<RLUnitConfig> 
			(rlConfig->rlUnitConfig[1])->fileName.c_str(), 
			expectedFileNameUnit1.c_str());
		EXPECT_STREQ (static_pointer_cast<RLUnitConfig> 
			(rlConfig->rlUnitConfig[2])->fileName.c_str(),
			expectedFileNameUnit2.c_str());
		EXPECT_STREQ (static_pointer_cast<RLUnitConfig> 
			(rlConfig->rlUnitConfig[3])->fileName.c_str(),
			expectedFileNameUnit3.c_str());
    }
}

TEST (RLV12ConfigProcessorTest, spinUpTimeCorrectlyDefaulted)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RL]\n"
		"units = 4\n"
		"[RL.unit0]\n"
		"cabinet = 0/0\n"
		"[RL.unit1]\n"
		"cabinet = 0/0\n"
		"[RL.unit2]\n"
		"cabinet = 0/0\n"
		"[RL.unit3]\n"
		"cabinet = 0/0\n";

	stream >> ft;

	IniProcessor iniProcessor;

	// Verify the configuration is processed without errors
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig& configuration =
		iniProcessor.getSystemConfig ();

	// The first and only device in the configuration should be the RLV12
	ASSERT_TRUE (holds_alternative<shared_ptr<RLV12Config>> (configuration[0]));

	shared_ptr<RLV12Config> rlConfig =
		get<shared_ptr<RLV12Config>> (configuration[0]);

	// Verify the spin-up time of all four units is correctly defaulted
	EXPECT_EQ (static_pointer_cast<RLUnitConfig>
		(rlConfig->rlUnitConfig[0])->spinUpTime, 0);
	EXPECT_EQ (static_pointer_cast<RLUnitConfig>
		(rlConfig->rlUnitConfig[1])->spinUpTime, 0);
	EXPECT_EQ (static_pointer_cast<RLUnitConfig>
		(rlConfig->rlUnitConfig[2])->spinUpTime, 0);
	EXPECT_EQ (static_pointer_cast<RLUnitConfig>
		(rlConfig->rlUnitConfig[3])->spinUpTime, 0);
}

TEST (RLV12ConfigProcessorTest, spinUpTimeHasCorrectValues)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RL]\n"
		"units = 4\n"
		"[RL.unit0]\n"
		"spin-up-time = 0\n"
		"cabinet = 0/0\n"
		"[RL.unit1]\n"
		"cabinet = 0/0\n"
		"spin-up-time = 1\n"
		"[RL.unit2]\n"
		"cabinet = 0/0\n"
		"spin-up-time = 2\n"
		"[RL.unit3]\n"
		"cabinet = 0/0\n"
		"spin-up-time = 3\n";

	stream >> ft;

	IniProcessor iniProcessor;

	// Verify the configuration is processed without errors
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig& configuration =
		iniProcessor.getSystemConfig ();

	// The first and only device in the configuration should be the RLV12
	ASSERT_TRUE (holds_alternative<shared_ptr<RLV12Config>> (configuration[0]));

	shared_ptr<RLV12Config> rlConfig =
		get<shared_ptr<RLV12Config>> (configuration[0]);

	// Verify the spin-up time of all four units is correctly defaulted
	EXPECT_EQ (static_pointer_cast<RLUnitConfig>
		(rlConfig->rlUnitConfig[0])->spinUpTime, 0);
	EXPECT_EQ (static_pointer_cast<RLUnitConfig>
		(rlConfig->rlUnitConfig[1])->spinUpTime, 1);
	EXPECT_EQ (static_pointer_cast<RLUnitConfig>
		(rlConfig->rlUnitConfig[2])->spinUpTime, 2);
	EXPECT_EQ (static_pointer_cast<RLUnitConfig>
		(rlConfig->rlUnitConfig[3])->spinUpTime, 3);
}

TEST (RLV12ConfigProcessorTest, unitNumberCorrectlySet)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RL]\n"
		"units = 4\n"
		"[RL.unit0]\n"
		"cabinet = 0/0\n"
		"[RL.unit1]\n"
		"cabinet = 0/0\n"
		"[RL.unit2]\n"
		"cabinet = 0/0\n"
		"[RL.unit3]\n"
		"cabinet = 0/0\n";

	stream >> ft;

	IniProcessor iniProcessor;

	// Verify the configuration is processed without errors
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig& configuration =
		iniProcessor.getSystemConfig ();

	// The first and only device in the configuration should be the RLV12
	ASSERT_TRUE (holds_alternative<shared_ptr<RLV12Config>> (configuration[0]));

	shared_ptr<RLV12Config> rlConfig =
		get<shared_ptr<RLV12Config>> (configuration[0]);

	// Verify the spin-up time of all four units is correctly defaulted
	EXPECT_EQ (static_pointer_cast<RLUnitConfig>
		(rlConfig->rlUnitConfig[0])->unitNumber, 0);
	EXPECT_EQ (static_pointer_cast<RLUnitConfig>
		(rlConfig->rlUnitConfig[1])->unitNumber, 1);
	EXPECT_EQ (static_pointer_cast<RLUnitConfig>
		(rlConfig->rlUnitConfig[2])->unitNumber, 2);
	EXPECT_EQ (static_pointer_cast<RLUnitConfig>
		(rlConfig->rlUnitConfig[3])->unitNumber, 3);
}