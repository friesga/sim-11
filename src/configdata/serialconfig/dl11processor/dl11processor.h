#ifndef _DL11PROCESSOR_H_
#define _DL11PROCESSOR_H_

#include "configdata/sectionprocessor/deviceconfigprocessor.h"
#include "configdata/serialconfig/dl11config/dl11config.h"
#include "configdata/serialconfig/uartconfig/uartconfig.h"

#include <map>

using std::map;

class DL11Processor : public DeviceConfigProcessor
{
public:
    // Functions required by the DeviceConfigProcessor interface
	DeviceConfig getConfig () override;

private:

	DL11Config dl11Config {};

	// Define process as a pointer to a DL11Processor member function
	// with a iniparser::Value argument and returning void.
	typedef void (DL11Processor::* Process)(iniparser::Value);

	map<string, Process> valueProcessors =
	{
		{"address", &DL11Processor::processAddress},
		{"vector", &DL11Processor::processVector},
	};

	void processAddress (iniparser::Value value);
	void processVector (iniparser::Value value);
};

#endif // _DL11PROCESSOR_H_