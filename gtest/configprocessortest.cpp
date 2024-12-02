#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/rl/rlconfig/rlconfig.h"

#include <fstream>	
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <ranges>

using std::shared_ptr;
using std::vector;
using std::string;
using std::ranges::find_if;

// Verify all sections in the testset are processed once and can be retrieved
TEST (ConfigProcessorTest, allSectionsProcessedOnce)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RLV12]\n"
		"[MSV11]\n"
		"[DLV11-J]\n"
		"[BDV11]\n"
		"[RXV21]\n"
		"[BA11-N]\n"
		"cabinet = 0/0\n";
		
	stream >> ft;

	SystemConfig allDevices
	{
		shared_ptr<MSV11Config> {},
		shared_ptr<DLV11JConfig> {},
		shared_ptr<BDV11Config> {},
		shared_ptr<RXV21Config> {},
		shared_ptr<RLV12Config> {},
		shared_ptr<BA11_NConfig> {}
	};

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	for (auto thisDevice : iniProcessor.getSystemConfig ())
    {
		auto findType = [thisDevice] (auto device)
			{return var_type (device) == var_type (thisDevice); };

        auto foundIterator = find_if (allDevices, findType);

		// Verify the section has not already been processed
		EXPECT_TRUE (foundIterator != allDevices.end());

		// Mark the device types processed
		allDevices.remove (foundIterator);
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
	stream << "[RLV12]\n"
		"[RLV12.unit4]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Unit number out of range 0-3");
	}
	catch (...)
	{
		FAIL();
	}
}