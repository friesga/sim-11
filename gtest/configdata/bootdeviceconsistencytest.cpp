#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/systemconfig/systemconfig.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/consistencychecker/consistencychecker.h"
#include "configdata/kt24/kt24config/kt24config.h"

#include <fstream>
#include <gtest/gtest.h>

class CaptureStdout
{
public:
	CaptureStdout ()
	{
        // Save the original stream buffer of std::cout and
		// redirect std::cout to strCout.
        oldCoutStreamBuf = std::cout.rdbuf ();
        std::cout.rdbuf (strCout.rdbuf ());
	}

	~CaptureStdout ()
	{
        // Restore the original stream buffer of std::cout
        std::cout.rdbuf (oldCoutStreamBuf);
	}

	std::string getOutput ()
	{
        return strCout.str ();
	}

private:
	std::streambuf* oldCoutStreamBuf;
    std::ostringstream strCout;
};

// Verify that a warning is given if no bootable device is configured
TEST (BootDeviceConsistencyTest, noBootableDeviceConfigured)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KD11-NA]\n"
		"[MSV11]\n"
		"[DLV11-J]\n"
        "[BDV11]\n"
		"[BA11-N]\n"
		"cabinet = 0/19\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};

    CaptureStdout captureStdout;
    try
    {
        consistencyChecker.checkQbusConsistency ();
    }
    catch (...)
    {
        FAIL ();
    }
	EXPECT_STREQ (captureStdout.getOutput ().c_str (),
		"Warning: No removable media configured. This system cannot boot.\n");
}

TEST (BootDeviceConsistencyTest, rxv21CountsAsBootableDevice)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KD11-NA]\n"
		"[MSV11]\n"
		"[DLV11-J]\n"
		"[BDV11]\n"
		"[BA11-N]\n"
		"cabinet = 0/19\n"
		"[RXV21]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};

	CaptureStdout captureStdout;
	try
	{
		consistencyChecker.checkQbusConsistency ();
	}
	catch (...)
	{
		FAIL ();
	}
	EXPECT_STREQ (captureStdout.getOutput ().c_str (), "");
}


TEST (BootDeviceConsistencyTest, rlv11CountsAsBootableDevice)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KD11-NA]\n"
		"[MSV11]\n"
		"[DLV11-J]\n"
		"[BDV11]\n"
		"[BA11-N]\n"
		"cabinet = 0/19\n"
		"[RLV11]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};

	CaptureStdout captureStdout;
	try
	{
		consistencyChecker.checkQbusConsistency ();
	}
	catch (...)
	{
		FAIL ();
	}
	EXPECT_STREQ (captureStdout.getOutput ().c_str (), "");
}

TEST (BootDeviceConsistencyTest, rlv12CountsAsBootableDevice)
{
	iniparser::File ft;
	std::stringstream stream;
	stream << "[KD11-NA]\n"
		"[MSV11]\n"
		"[DLV11-J]\n"
		"[BDV11]\n"
		"[BA11-N]\n"
		"cabinet = 0/19\n"
		"[RLV12]\n";
	stream >> ft;

	IniProcessor iniProcessor;
	EXPECT_NO_THROW (iniProcessor.process (ft));

	SystemConfig systemConfig = iniProcessor.getSystemConfig ();
	ConsistencyChecker consistencyChecker {systemConfig};

	CaptureStdout captureStdout;
	try
	{
		consistencyChecker.checkQbusConsistency ();
	}
	catch (...)
	{
		FAIL ();
	}
	EXPECT_STREQ (captureStdout.getOutput ().c_str (), "");
}