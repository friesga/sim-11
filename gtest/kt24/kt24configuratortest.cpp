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