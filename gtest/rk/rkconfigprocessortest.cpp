#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/rk/rk11d/rk11dconfig/rk11dconfig.h"

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

TEST (RKConfigProcessorTest, configProcessed)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RK11-D]\n"
		"address = 0177400\n"
		"vector = 0220\n"
		"bus_request_level = 5\n"
		"units = 1\n"

		"[RK11-D.RK05-0]\n"
		"cabinet = 0/0\n"
		"filename = rk05.dsk\n"
		"newfile = true\n"
		"write-protect = false\n";

	stream >> ft;

	IniProcessor iniProcessor;

	// Verify the configuration is processed without errors
	EXPECT_NO_THROW (iniProcessor.process (ft));

	// Verify the device is present in the configuration with all attributes
	// having their correct value.
	SystemConfig configuration =
		iniProcessor.getSystemConfig ();

	ASSERT_TRUE (holds_alternative<shared_ptr<RK11DConfig>> (configuration[0]));

	shared_ptr<RK11DConfig> rk11dConfig =
		get<shared_ptr<RK11DConfig>> (configuration[0]);

	EXPECT_EQ (rk11dConfig->address, 0177400);
	EXPECT_EQ (rk11dConfig->vector, 0220);
	EXPECT_EQ (rk11dConfig->busRequestLevel, 5);
	EXPECT_EQ (rk11dConfig->numUnits, 1);

	EXPECT_EQ (static_pointer_cast<RK05Config>
		(rk11dConfig->rk05Config[0])->writeProtect, false);
}

TEST (RKConfigProcessorTest, configProcessorThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[RK11-D]\n"
		"unknown_key = XXX\n";

	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Unknown key in section RK11-D: unknown_key");
	}
	catch (...)
	{
		FAIL ();
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

TEST (RKConfigProcessorTest, fileName)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[RK11-D]\n"
		"units = 4\n"
		"[RK11-D.RK05-0]\n"
		"cabinet = 0/0\n"
		"filename = \\somefile\n"
		"[RK11-D.RK05-1]\n"
		"cabinet = 0/0\n"
		"filename = Windows:{G:\\windowsFileName}, Linux:{/mnt/g/sim-11/linuxFileName\n"
		"[RK11-D.RK05-2]\n"
		"cabinet = 0/0\n"
		"filename = Linux:linuxFileName, unqualifiedName\n"
		"[RK11-D.RK05-3]\n"
		"cabinet = 0/0\n"
		"filename = Windows:windowsFileName, unqualifiedName\n";

	stream >> ft;

	IniProcessor iniProcessor;
	iniProcessor.process (ft);

	for (auto device : iniProcessor.getSystemConfig ())
	{
		// The only device type in this testset is the RK11-D so if that's
		// not corrected the following tests will fail too.
		ASSERT_TRUE (holds_alternative<shared_ptr<RK11DConfig>> (device));

		// The device's type is RK11-D so the configuration is a RK11DConfig
		auto rk11dConfig =
			get<shared_ptr<RK11DConfig>> (device);

		// Now we can check the unit's filenames. The devices in the 
		// units are of type RK05Config.
		EXPECT_STREQ (static_pointer_cast<RK05Config>
			(rk11dConfig->rk05Config[0])->fileName.c_str (), "somefile");
		EXPECT_STREQ (static_pointer_cast<RK05Config>
			(rk11dConfig->rk05Config[1])->fileName.c_str (),
			expectedFileNameUnit1.c_str ());
		EXPECT_STREQ (static_pointer_cast<RK05Config>
			(rk11dConfig->rk05Config[2])->fileName.c_str (),
			expectedFileNameUnit2.c_str ());
		EXPECT_STREQ (static_pointer_cast<RK05Config>
			(rk11dConfig->rk05Config[3])->fileName.c_str (),
			expectedFileNameUnit3.c_str ());
	}
}

TEST (RKConfigProcessorTest, spinUpTimeCorrectlyDefaulted)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RK11-D]\n"
		"units = 4\n"
		"[RK11-D.RK05-0]\n"
		"cabinet = 0/0\n"
		"[RK11-D.RK05-1]\n"
		"cabinet = 0/0\n"
		"[RK11-D.RK05-2]\n"
		"cabinet = 0/0\n"
		"[RK11-D.RK05-3]\n"
		"cabinet = 0/0\n";

	stream >> ft;

	IniProcessor iniProcessor;

	// Verify the configuration is processed without errors
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig configuration =
		iniProcessor.getSystemConfig ();

	// The first and only device in the configuration should be the RK11-D
	ASSERT_TRUE (holds_alternative<shared_ptr<RK11DConfig>> (configuration[0]));

	shared_ptr<RK11DConfig> rk11dConfig =
		get<shared_ptr<RK11DConfig>> (configuration[0]);

	// Verify the spin-up time of all four units is correctly defaulted
	EXPECT_EQ (static_pointer_cast<RK05Config>
		(rk11dConfig->rk05Config[0])->spinUpTime, 0);
	EXPECT_EQ (static_pointer_cast<RK05Config>
		(rk11dConfig->rk05Config[1])->spinUpTime, 0);
	EXPECT_EQ (static_pointer_cast<RK05Config>
		(rk11dConfig->rk05Config[2])->spinUpTime, 0);
	EXPECT_EQ (static_pointer_cast<RK05Config>
		(rk11dConfig->rk05Config[3])->spinUpTime, 0);
}

TEST (RKConfigProcessorTest, spinUpTimeHasCorrectValues)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RK11-D]\n"
		"units = 4\n"
		"[RK11-D.RK05-0]\n"
		"spin-up-time = 0\n"
		"cabinet = 0/0\n"
		"[RK11-D.RK05-1]\n"
		"cabinet = 0/0\n"
		"spin-up-time = 1\n"
		"[RK11-D.RK05-2]\n"
		"cabinet = 0/0\n"
		"spin-up-time = 2\n"
		"[RK11-D.RK05-3]\n"
		"cabinet = 0/0\n"
		"spin-up-time = 3\n";

	stream >> ft;

	IniProcessor iniProcessor;

	// Verify the configuration is processed without errors
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig configuration =
		iniProcessor.getSystemConfig ();

	// The first and only device in the configuration should be the RK11-D
	ASSERT_TRUE (holds_alternative<shared_ptr<RK11DConfig>> (configuration[0]));

	shared_ptr<RK11DConfig> rk11dConfig =
		get<shared_ptr<RK11DConfig>> (configuration[0]);

	// Verify the spin-up time of all four units is correctly defaulted
	EXPECT_EQ (static_pointer_cast<RK05Config>
		(rk11dConfig->rk05Config[0])->spinUpTime, 0);
	EXPECT_EQ (static_pointer_cast<RK05Config>
		(rk11dConfig->rk05Config[1])->spinUpTime, 1);
	EXPECT_EQ (static_pointer_cast<RK05Config>
		(rk11dConfig->rk05Config[2])->spinUpTime, 2);
	EXPECT_EQ (static_pointer_cast<RK05Config>
		(rk11dConfig->rk05Config[3])->spinUpTime, 3);
}

TEST (RKConfigProcessorTest, unitNumberCorrectlySet)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RK11-D]\n"
		"units = 4\n"
		"[RK11-D.RK05-0]\n"
		"cabinet = 0/0\n"
		"[RK11-D.RK05-1]\n"
		"cabinet = 0/0\n"
		"[RK11-D.RK05-2]\n"
		"cabinet = 0/0\n"
		"[RK11-D.RK05-3]\n"
		"cabinet = 0/0\n";

	stream >> ft;

	IniProcessor iniProcessor;

	// Verify the configuration is processed without errors
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig configuration =
		iniProcessor.getSystemConfig ();

	// The first and only device in the configuration should be the RK11-D
	ASSERT_TRUE (holds_alternative<shared_ptr<RK11DConfig>> (configuration[0]));

	shared_ptr<RK11DConfig> rk11dConfig =
		get<shared_ptr<RK11DConfig>> (configuration[0]);

	// Verify the spin-up time of all four units is correctly defaulted
	EXPECT_EQ (static_pointer_cast<RK05Config>
		(rk11dConfig->rk05Config[0])->unitNumber, 0);
	EXPECT_EQ (static_pointer_cast<RK05Config>
		(rk11dConfig->rk05Config[1])->unitNumber, 1);
	EXPECT_EQ (static_pointer_cast<RK05Config>
		(rk11dConfig->rk05Config[2])->unitNumber, 2);
	EXPECT_EQ (static_pointer_cast<RK05Config>
		(rk11dConfig->rk05Config[3])->unitNumber, 3);
}


TEST (RKConfigProcessorTest, unitNumberOutOfRangeThrows)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RK11-D]\n"
		"units = 1\n"
		"[RK11-D.RK05-8]\n";

	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Unit number out of range 0-7: RK05-8");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (RKConfigProcessorTest, invalidUnitNumberThrows)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RK11-D]\n"
		"units = 1\n"
		"[RK11-D.RK05-A]\n";

	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Invalid unit number: RK05-A");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (RKConfigProcessorTest, busRequestLevelOutOfRangeThrows)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RK11-D]\n"
		"bus_request_level = 0\n";

	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "RK11-D bus request level must be between 4 and 7");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (RKConfigProcessorTest, addressOutOfRangeThrows)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RK11-D]\n"
		"address = 0157760\n";

	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "RK11-D address must be in the range 0160000-0177770");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (RKConfigProcessorTest, addressOnIncorrectBoundaryThrows)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RK11-D]\n"
		"address = 0160002\n";

	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "RK11-D address must be on a eight byte boundary");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (RKConfigProcessorTest, vectorOutOfRangeThrows)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RK11-D]\n"
		"vector = 01000\n";

	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "RK11-D vector must be in the range 0000-0740");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (RKConfigProcessorTest, oddVectorThrows)
{
	std::stringstream stream;
	iniparser::File ft;

	stream << "[RK11-D]\n"
		"vector = 0221\n";

	stream >> ft;

	IniProcessor iniProcessor;
	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "RK11-D vector must be even");
	}
	catch (...)
	{
		FAIL ();
	}
}