#ifndef _RK11DPROCESSOR_H_
#define _RK11DPROCESSOR_H_

#include "configdata/sectionprocessor/deviceconfigprocessor.h"
#include "configdata/rk/rk11d/rk11dconfig/rk11dconfig.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::shared_ptr;
using std::map;
using std::string;

// Definition a configuration processor for the RK11-D options.
class RK11DProcessor : public DeviceConfigProcessor
{
	unique_ptr<RK11DConfig> rk11dConfigPtr {nullptr};

	// Define process as a pointer to a RK11DProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RK11DProcessor::* Process)(iniparser::Value);

	map<string, Process> valueProcessors =
	{
		{"address", &RK11DProcessor::processAddress},
		{"vector", &RK11DProcessor::processVector},
		{"units", &RK11DProcessor::processUnits},
		{"bus_request_level", &RK11DProcessor::processBRLevel}
	};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void processAddress (iniparser::Value value);
	void processVector (iniparser::Value value);
	void processUnits (iniparser::Value value);
	void processBRLevel (iniparser::Value value);

public:
	RK11DProcessor ();
	void processSubsection (iniparser::Section* subSection);
	void checkConsistency ();
	DeviceConfig getConfig ();
};

#endif // _RK11DPROCESSOR_H_