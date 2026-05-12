#include "configdata/systemconfig/systemconfig.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/mm11_econfig/mm11_econfig.h"
#include "pdp11/pdp11.h"
#include "mem/mm11e/mm11e.h"

#include <gtest/gtest.h>

// This test configures two MM11-E devices with a hole in the adress space
// from 020000 upto 040000 to verify that the starting address and bus addresses
// are correctly processed.
TEST (MM11ETest, mm11EcapacityIsCorrect)
{
	Qbus bus;
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MM11-E]\n"
		"starting_address = 000000\n";

	stream << "[MM11-E]\n"
		"starting_address = 040000\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

    // Verify two MM11-E devices are configured
	ASSERT_TRUE (holds_alternative<MM11EConfig> (systemConfig[0]));
	ASSERT_TRUE (holds_alternative<MM11EConfig> (systemConfig[1]));

	// The device's type is MM11-E so the configuration is a MM11-E object
	auto mm11eConfig0 = get<MM11EConfig> (systemConfig[0]);
	auto mm11eConfig1 = get<MM11EConfig> (systemConfig[1]);

    // Verify the devices have the correct starting addresses
	EXPECT_EQ (mm11eConfig0.startingAddress, 000000);
	EXPECT_EQ (mm11eConfig1.startingAddress, 040000);

    // Create the MM11-E devices based on the configuration
	MM11E mm11e0 {&bus, mm11eConfig0};
	MM11E mm11e1 {&bus, mm11eConfig1};

	u16 dataWritten {0177777};
	CondData<u16> dataRead {0};

    // Verify the last word of the first MM11-E can be accessed
	ASSERT_TRUE (mm11e0.responsible (BusAddress (017776, BusAddress::Width::_16Bit)));
	EXPECT_EQ (mm11e0.writeWord (BusAddress (017776, BusAddress::Width::_16Bit),
		dataWritten), StatusCode::Success);
	dataRead = mm11e0.read (BusAddress (017776, BusAddress::Width::_16Bit));
	EXPECT_EQ (dataWritten, dataRead);
	EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);

    // Verify the address space of the first MM11-E ends at 020000
	EXPECT_FALSE (mm11e0.responsible (BusAddress (020000, BusAddress::Width::_16Bit)));

    // Verify the first word of the second MM11-E can be accessed
	ASSERT_TRUE (mm11e1.responsible (BusAddress (040000, BusAddress::Width::_16Bit)));
	EXPECT_EQ (mm11e1.writeWord (BusAddress (040000, BusAddress::Width::_16Bit),
		dataWritten), StatusCode::Success);
	dataRead = mm11e1.read (BusAddress (040000, BusAddress::Width::_16Bit));
	EXPECT_EQ (dataWritten, dataRead);
	EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);
	
	// Verify the last word of the second MM11-E can be accessed
	ASSERT_TRUE (mm11e1.responsible (BusAddress (057776, BusAddress::Width::_16Bit)));
	EXPECT_EQ (mm11e1.writeWord (BusAddress (057776, BusAddress::Width::_16Bit),
		dataWritten), StatusCode::Success);
	dataRead = mm11e1.read (BusAddress (057776, BusAddress::Width::_16Bit));
	EXPECT_EQ (dataWritten, dataRead);
	EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);

	// Verify the address space of the first MM11-E ends at 060000
	EXPECT_FALSE (mm11e1.responsible (BusAddress (060000, BusAddress::Width::_16Bit)));
}

// This test configures two MM11-EX devices with a hole in the adress space
// from 040000 upto 060000 to verify that the starting address and bus addresses
// are correctly processed.
TEST (MM11ETest, mm11EXcapacityIsCorrect)
{
	Qbus bus;
	iniparser::File ft;
	std::stringstream stream;
	stream << "[MM11-E]\n"
		"starting_address = 000000\n"
		"memory_extension = true\n";

	stream << "[MM11-E]\n"
		"starting_address = 060000\n"
		"memory_extension = true\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig =
		iniProcessor.getSystemConfig ();

	// Verify two MM11-E devices are configured
	ASSERT_TRUE (holds_alternative<MM11EConfig> (systemConfig[0]));
	ASSERT_TRUE (holds_alternative<MM11EConfig> (systemConfig[1]));

	// The device's type is MM11-E so the configuration is a MM11-E object
	auto mm11eConfig0 = get<MM11EConfig> (systemConfig[0]);
	auto mm11eConfig1 = get<MM11EConfig> (systemConfig[1]);

	// Verify the devices have the correct starting addresses
	EXPECT_EQ (mm11eConfig0.startingAddress, 000000);
	EXPECT_EQ (mm11eConfig1.startingAddress, 060000);

	// Create the MM11-E devices based on the configuration
	MM11E mm11e0 {&bus, mm11eConfig0};
	MM11E mm11e1 {&bus, mm11eConfig1};

	u16 dataWritten {0177777};
	CondData<u16> dataRead {0};

	// Verify the last word of the first MM11-E can be accessed
	ASSERT_TRUE (mm11e0.responsible (BusAddress (037776, BusAddress::Width::_16Bit)));
	EXPECT_EQ (mm11e0.writeWord (BusAddress (037776, BusAddress::Width::_16Bit),
		dataWritten), StatusCode::Success);
	dataRead = mm11e0.read (BusAddress (037776, BusAddress::Width::_16Bit));
	EXPECT_EQ (dataWritten, dataRead);
	EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);

	// Verify the address space of the first MM11-E ends at 040000
	EXPECT_FALSE (mm11e0.responsible (BusAddress (040000, BusAddress::Width::_16Bit)));

	// Verify the first word of the second MM11-E can be accessed
	ASSERT_TRUE (mm11e1.responsible (BusAddress (060000, BusAddress::Width::_16Bit)));
	EXPECT_EQ (mm11e1.writeWord (BusAddress (060000, BusAddress::Width::_16Bit),
		dataWritten), StatusCode::Success);
	dataRead = mm11e1.read (BusAddress (060000, BusAddress::Width::_16Bit));
	EXPECT_EQ (dataWritten, dataRead);
	EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);

	// Verify the last word of the second MM11-E can be accessed
	ASSERT_TRUE (mm11e1.responsible (BusAddress (0117776, BusAddress::Width::_16Bit)));
	EXPECT_EQ (mm11e1.writeWord (BusAddress (0117776, BusAddress::Width::_16Bit),
		dataWritten), StatusCode::Success);
	dataRead = mm11e1.read (BusAddress (0117776, BusAddress::Width::_16Bit));
	EXPECT_EQ (dataWritten, dataRead);
	EXPECT_EQ (dataRead.statusCode (), StatusCode::Success);

	// Verify the address space of the first MM11-E ends at 0120000
	EXPECT_FALSE (mm11e1.responsible (BusAddress (0120000, BusAddress::Width::_16Bit)));
}