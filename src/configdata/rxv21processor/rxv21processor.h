#ifndef _RXPROCESSOR_H_
#define _RXPROCESSOR_H_

#include "../sectionprocessor/sectionprocessor.h"
#include "../rxv21config/rxv21config.h"

class RXV21Processor : public SectionProcessor
{
    std::unique_ptr<RXV21Config> rxConfigPtr {nullptr};

	// Define process as a pointer to a RlProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RXV21Processor::*Process)(iniparser::Value);
	
	std::map<std::string, Process> valueProcessors =
	{
		{"address", &RXV21Processor::processAddress},
		{"vector", &RXV21Processor::processVector}
	};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void processAddress (iniparser::Value value);
	void processVector (iniparser::Value value);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);


public:
	RXV21Processor ();
	shared_ptr<DeviceConfig> getConfig ();
};


#endif // !_RXPROCESSOR_H_
