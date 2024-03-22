#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/devicetype/devicetype.h"
#include "configdata/rlconfig/rlconfig.h"

#include <fstream>	
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <variant>
#include <ranges>

using std::shared_ptr;
using std::static_pointer_cast;
using std::vector;
using std::string;
using std::get;
using std::ranges::find_if;

// Minimal tests for the ConfigProcessor class
TEST (ConfigProcessorTest, configProcessed)
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
		"filename = rl01.dsk\n"
		"newfile = true\n"
		"read-only = false\n";
		
	stream >> ft;

	IniProcessor iniProcessor;

	// Verify the configuration is processed without errors
	EXPECT_NO_THROW (iniProcessor.process (ft));

	// Verify the device is present in the configuration with all attributes
	// having their correct value.
	vector<DeviceConfigVariant> &configuration = 
		iniProcessor.getSystemConfig ();

	shared_ptr<RLConfig> rlConfig = 
		get<shared_ptr<RLConfig>> (configuration[0]);

	EXPECT_EQ (rlConfig->deviceType_, DeviceType::RLV12);
	EXPECT_EQ (rlConfig->rlType, RLConfig::RLType::RLV12);
	EXPECT_EQ (rlConfig->address, 0174400);
	EXPECT_EQ (rlConfig->vector, 0160);
	EXPECT_EQ (rlConfig->numUnits, 1);
}

TEST (ConfigProcessorTest, configProcessorThrows)
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

	IniProcessor iniProcessor;
	iniProcessor.process (ft);

	for (auto device : iniProcessor.getSystemConfig ())
    {
		// The device's type is RLV12 so the configuration is a RLConfig
		auto rlConfig = 
			get<shared_ptr<RLConfig>> (device);

		// The only device type in this testset is the RLV12 so if that's
		// not corrected the following tests will fail too.
		ASSERT_EQ (rlConfig->deviceType_, DeviceType::RLV12);

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

// Verify all sections in the testset are processed once and can be retrieved
TEST (ConfigProcessorTest, allSectionsProcessedOnce)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RL]\n"
		"[MSV11]\n"
		"[DLV11-J]\n"
		"[BDV11]\n"
		"[RXV21]\n"
		"[BA11-N]\n";
		
	stream >> ft;

	vector<DeviceConfigVariant> allDevices
	{
		shared_ptr<MSV11Config> {},
		shared_ptr<DLV11JConfig> {},
		shared_ptr<BDV11Config> {},
		shared_ptr<RXV21Config> {},
		shared_ptr<RLConfig> {},
		shared_ptr<BA11_NConfig> {}
	};

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	for (auto thisDevice : iniProcessor.getSystemConfig ())
    {
		// The only device type in this testset is the RLV12 so if that's
		// not corrected the following tests will fail too.
		// ASSERT_EQ (device->deviceType_, DeviceType::RLV12);
		
		auto findType = [thisDevice] (auto device)
			{return var_type (device) == var_type (thisDevice); };

        auto foundIterator = find_if (allDevices, findType);

		// Verify the section has not already been processed
		EXPECT_TRUE (foundIterator != allDevices.end());

		// Mark the device types processed
		allDevices.erase (foundIterator);
	}

	// Verify all devices are processed
	EXPECT_TRUE (allDevices.empty ());
}

TEST (ConfigProcessorTest, unsupportedSectionThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[UNKNOWN]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (string const &msg)
	{
		EXPECT_STREQ (msg.c_str(), "Unsupported section: UNKNOWN");
	}
	catch (...)
	{
		FAIL();
	}
}


TEST (ConfigProcessorTest, outOfRangeUnitNumberThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[RL]\n"
		"[RL.unit4]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "RL unit number out of range 0-3");
	}
	catch (...)
	{
		FAIL();
	}
}