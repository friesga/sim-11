#include "configdata/configprocessor/configprocessor.h"
#include "configdata/include/configdata.h"

#include <sstream>	
#include <gtest/gtest.h>

// Minimal tests for the ConfigProcessor class
TEST (RxConfiguratorTest, rxConfigProcessed)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[RX]\n"
		"address = 0174400\n"
		"vector = 0160\n"

		"[RX.unit0]\n"
		"filename = rx01.dsk\n";
		
	stream >> ft;

	ConfigData configProcessor;
	EXPECT_NO_THROW (configProcessor.process (ft));

	DeviceConfig *deviceConfig = configProcessor.getConfig ();
	EXPECT_EQ (deviceConfig->rxConfig->address, 0174400);
	EXPECT_EQ (deviceConfig->rxConfig->vector, 0160);
	EXPECT_EQ (deviceConfig->rxConfig->rxUnitConfig[0].fileName, "rx01.dsk");
}
