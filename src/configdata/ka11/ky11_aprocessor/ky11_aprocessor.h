#ifndef _KY11_APROCESSOR_H_
#define _KY11_APROCESSOR_H_

#include "configdata/sectionprocessor/unitconfigprocessor.h"
#include "configdata/ka11/ky11_aconfig/ky11_aconfig.h"

#include <map>

using std::map;

class KY11_AProcessor : public UnitConfigProcessor
{
public:
    KY11_AProcessor (iniparser::Section* subSection);

    // Functions required by the SectionProcessor base class
    void processValue (iniparser::Section::ValueIterator valueIterator);
    void checkConsistency ();
    void processSubsection (iniparser::Section* subSection);

	KY11_AConfig getConfig ();

private:
	KY11_AConfig ky11_aConfig_ {};

	// Define process as a pointer to a KY11_AProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (KY11_AProcessor::* Process)(iniparser::Value);

	map<string, Process> valueProcessors =
	{
		{"cabinet", &KY11_AProcessor::processCabinet}
	};

    void checkType (string type);
	void processCabinet (iniparser::Value value);
};

#endif // _KY11_APROCESSOR_H_