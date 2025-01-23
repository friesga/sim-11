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

TEST (KT24ConfiguratorTest, m9312OptionsAccepted)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KT24]\n"
		"diag-rom = 23-248F1\n"
		"boot-roms = 23-751A9, 23-752A9, 23-753A9, 23-755A9\n"
		"starting-address = 0173000\n"
		"power-up-boot-enable = true\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// The only device type in this testset is the M9312 so if that's
	// not correct the following tests will fail too.
	ASSERT_TRUE (holds_alternative<shared_ptr<KT24Config>> (systemConfig[0]));

	// The device's type is KT24 so the configuration is a KT24Config object
	shared_ptr<KT24Config> kt24Config =
		get<shared_ptr<KT24Config>> (systemConfig[0]);

	EXPECT_EQ (kt24Config->m9312ConfigPtr->diagnosticROM,
		M9312Config::DiagROMType::_23_248F1);
	EXPECT_EQ (kt24Config->m9312ConfigPtr->bootROM[0],
		M9312Config::BootROMType::_23_751A9);
	EXPECT_EQ (kt24Config->m9312ConfigPtr->bootROM[1],
		M9312Config::BootROMType::_23_752A9);
	EXPECT_EQ (kt24Config->m9312ConfigPtr->bootROM[2],
		M9312Config::BootROMType::_23_753A9);
	EXPECT_EQ (kt24Config->m9312ConfigPtr->bootROM[3],
		M9312Config::BootROMType::_23_755A9);
	EXPECT_EQ (kt24Config->m9312ConfigPtr->startingAddress, 0173000);
	EXPECT_TRUE (kt24Config->m9312ConfigPtr->powerUpBootEnable);
}

TEST (KT24ConfiguratorTest, invalidOptionThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KT24]\n"
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
		EXPECT_STREQ (except.what (), "Unknown key in section KT24: unknown-option");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (KT24ConfiguratorTest, invalidDiagROMThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KT24]\n"
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
		EXPECT_STREQ (except.what (), "Incorrect diagnostic rom in KT24: 23-751A9");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (KT24ConfiguratorTest, tooManyBootROMsThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KT24]\n"
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
		EXPECT_STREQ (except.what (), "KT24 can have at most four boot ROMs");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (KT24ConfiguratorTest, invalidBootROMThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KT24]\n"
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
		EXPECT_STREQ (except.what (), "Incorrect boot rom in KT24: 23-751B9");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (KT24ConfiguratorTest, incorrectStartingAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KT24]\n"
		"starting-address = xyz\n";
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
			"Incorrect starting address in KT24 section specified: xyz");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (KT24ConfiguratorTest, invalidStartingAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KT24]\n"
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
			"KT24 starting address must be in the range 0173000 to 0174000 or 0165000 to 0166000");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (KT24ConfiguratorTest, oddStartingAddressThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KT24]\n"
		"starting-address = 0173001\n";
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
			"KT24 starting address must be even");
	}
	catch (...)
	{
		FAIL ();
	}
}

TEST (KT24ConfiguratorTest, invalidPowerUpBootEnableThrows)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KT24]\n"
		"power-up-boot-enable = xxx\n";
	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL ();
	}
	catch (std::invalid_argument const& except)
	{
		EXPECT_STREQ (except.what (), "KT24 power-up-boot-enable must be either true or false");
	}
	catch (...)
	{
		FAIL ();
	}
}

