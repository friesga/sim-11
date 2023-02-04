#ifndef _RXV21UNITCONFIG_H_
#define _RXV21UNITCONFIG_H_

#include "../deviceconfig/deviceconfig.h"

#include <string>

using std::string;

// Definition of the configuration result of an RXV21 unit.
struct RXV21UnitConfig : public DeviceConfig
{
	string fileName {};
	bool newFile {false};
	bool readOnly {false};
	bool overwrite {false};

    RXV21UnitConfig ();
};


#endif // !_RXV21UNITCONFIG_H_
