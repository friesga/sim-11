#include "dl11processor.h"
#include "touint.h"

using std::invalid_argument;

void DL11Processor::processAddress (iniparser::Value value)
{
	try
	{
		dl11Config.baseAddress = touint<u16> (value.asString ());
	}
	catch (std::invalid_argument const&)
	{
		throw invalid_argument {"Incorrect address in DL11 section specified: " +
			value.asString ()};
	}
}

void DL11Processor::processVector (iniparser::Value value)
{
	try
	{
		dl11Config.baseVector = touint<u16> (value.asString ());
	}
	catch (invalid_argument const&)
	{
		throw invalid_argument {"Incorrect vector in DL11 section specified: " +
			value.asString ()};
	}
}

DeviceConfig DL11Processor::getConfig ()
{
	return dl11Config;
}