#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/m9312/m9312config/m9312config.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (M9312ConfiguratorTest, optionsAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[M9312]\n"
		"diag-rom = 23-248F1\n"
		"boot-roms = 23-751A9, 23-752A9, 23-753A9, 23-755A9\n"
		"starting-address = 0173000\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the M9312 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<M9312Config>> (systemConfig[0]));

	// The device's type is M9312 so the configuration is a M9312Config object
	shared_ptr<M9312Config> m9312Config =
		get<shared_ptr<M9312Config>> (systemConfig[0]);

	EXPECT_EQ (m9312Config->diagnosticROM, M9312Config::ROMType::_23_248F1);
	EXPECT_EQ (m9312Config->bootROM[0], M9312Config::ROMType::_23_751A9);
	EXPECT_EQ (m9312Config->bootROM[1], M9312Config::ROMType::_23_752A9);
	EXPECT_EQ (m9312Config->bootROM[2], M9312Config::ROMType::_23_753A9);
    EXPECT_EQ (m9312Config->bootROM[3], M9312Config::ROMType::_23_755A9);
    EXPECT_EQ (m9312Config->startingAddress, 0173000);
}

TEST (M9312ConfiguratorTest, invalidOptionThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[M9312]\n"
		"unknown-option = XXX\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Unknown key in section M9312: unknown-option");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (M9312ConfiguratorTest, invalidDiagROMThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[M9312]\n"
		"diag-rom = 23-751A9\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Incorrect M9312 diagnostic rom: 23-751A9");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (M9312ConfiguratorTest, tooManyBootROMsThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[M9312]\n"
		"boot-roms = 23-751A9, 23-752A9, 23-753A9, 23-755A9, 23-756A9\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "M9312 can have at most four boot ROMs");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (M9312ConfiguratorTest, invalidBootROMThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[M9312]\n"
		"boot-roms = 23-751B9, 23-752A9, 23-753A9, 23-755A9\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "Incorrect M9312 boot rom: 23-751B9");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (M9312ConfiguratorTest, defaultInitialized)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[M9312]\n"
		"boot-roms = \n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the M9312 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<M9312Config>> (systemConfig[0]));

	// The device's type is M9312 so the configuration is a M9312Config object
	shared_ptr<M9312Config> m9312Config =
		get<shared_ptr<M9312Config>> (systemConfig[0]);

	EXPECT_EQ (m9312Config->diagnosticROM, M9312Config::ROMType::NONE);
	EXPECT_EQ (m9312Config->bootROM[0], M9312Config::ROMType::NONE);
	EXPECT_EQ (m9312Config->bootROM[1], M9312Config::ROMType::NONE);
	EXPECT_EQ (m9312Config->bootROM[2], M9312Config::ROMType::NONE);
	EXPECT_EQ (m9312Config->bootROM[3], M9312Config::ROMType::NONE);
	EXPECT_EQ (m9312Config->startingAddress, 0);
}

TEST (M9312ConfiguratorTest, invalidStartingAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[M9312]\n"
		"starting-address = 0174000\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (),
			"M9312 starting address must be in the range 0173000 to 0174000 or 0165000 to 0166000");
	}
	catch (...)
	{
		FAIL ();
	}
}
