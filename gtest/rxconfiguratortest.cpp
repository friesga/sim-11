#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/rxv21config/rxv21config.h"
#include "configdata/rxv21unitconfig/rxv21unitconfig.h"

#include <sstream>	
#include <gtest/gtest.h>

// Minimal tests for the ConfigProcessor class
TEST (RxConfiguratorTest, rxConfigProcessed)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[RXV21]\n"
		"address = 0174400\n"
		"vector = 0160\n"

		"[RXV21.unit0]\n"
		"filename = rx01.dsk\n";
		
	stream >> ft;

	IniProcessor configProcessor;
	EXPECT_NO_THROW (configProcessor.process (ft));

	vector<shared_ptr<DeviceConfig>> &deviceConfig = 
		configProcessor.getSystemConfig ();
	EXPECT_EQ (static_pointer_cast<RXV21Config> 
			(deviceConfig[0])->address, 0174400);
	EXPECT_EQ (static_pointer_cast<RXV21Config> 
			(deviceConfig[0])->vector, 0160);

	shared_ptr<RXV21Config> rxv21Config =
		static_pointer_cast<RXV21Config> (deviceConfig[0]);

	EXPECT_STREQ (static_pointer_cast<RXV21UnitConfig> 
			(rxv21Config->rxv21UnitConfig[0])->fileName.c_str(), 
			"rx01.dsk");

}
