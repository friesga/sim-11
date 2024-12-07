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