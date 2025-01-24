#ifndef _KT24_H_
#define _KT24_H_

#include "bus/include/bus.h"
#include "unibusmap.h"
#include "configdata/kt24/kt24config/kt24config.h"
#include "unibusmaplogic/unibusmaplogic.h"
#include "m9312/m9312.h"
#include "devicecommon/registerhandler/registerhandler.h"

#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

// The KT24 is the Unibus map option for the PDP-11/24. It provides standard
// Unibus map functionality, extended with M9312-compatible functions and ROM
// sockets a power supply voltage monitor.
//
class KT24 : public UnibusMap

{
public:
	KT24 (Bus* bus);
	KT24 (Bus* bus, shared_ptr<KT24Config> kt24Config);

    // Functions required for the BusDevice interface
	StatusCode read (BusAddress address, u16* destination);
	StatusCode writeWord (BusAddress address, u16 value);
	StatusCode writeByte (BusAddress address, u8 value);
	bool responsible (BusAddress address);
	void reset ();

	// Functions required for the UnibusMap interface
	BusAddress physicalAddressFrom18bitAddress (BusAddress address);
	void setControlLines (u16 controlLines);

	// The enable() and disable() functions are added to be able to control
	// the KT24 state from the unit tests.
	void enable ();
	void disable ();

private:
	unique_ptr<M9312> m9312_ {};
	unique_ptr<UnibusMapLogic> unibusMapLogic_ {};
	unique_ptr<RegisterHandler> registerHandler_ {};
};



#endif // _KT24_H_