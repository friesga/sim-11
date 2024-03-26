#ifndef _RXV21UNITCONFIG_H_
#define _RXV21UNITCONFIG_H_

#include <string>

using std::string;

// Definition of the configuration result of an RXV21 unit.
struct RXV21UnitConfig
{
	string fileName {};
	bool newFile {false};
	bool readOnly {false};
	bool overwrite {false};
};


#endif // !_RXV21UNITCONFIG_H_
