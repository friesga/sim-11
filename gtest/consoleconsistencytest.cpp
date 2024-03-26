#include "configdata/iniprocessor/iniprocessor.h"

#include <fstream>	
#include <gtest/gtest.h>

TEST (ConsoleConsistencyTest, twoConsolesThrows)
{
    iniparser::File ft;
	std::stringstream stream;
	stream << "[KDF11-B]\n"
		"[KDF11-B.SLU]\n"
		"slu1_enabled = true\n"
		"[DLV11-J]\n"
		"ch3_console_enabled = true";

	stream >> ft;

	IniProcessor iniProcessor;

	try
	{
		iniProcessor.process (ft);
		FAIL();
	}
	catch (std::invalid_argument const &except)
	{
		EXPECT_STREQ (except.what(), "Specify either KDF11-B or DLV11-J as console");
	}
	catch (...)
	{
		FAIL();
	}
}