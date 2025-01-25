#ifndef _PDP11_H_
#define _PDP11_H_

#include "kd/include/pdp11processor.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/systemconfig/systemconfig.h"
#include "memorydevice.h"
#include "ba11_n/ba11_n.h"
#include "ba11_l/ba11_l.h"
#include "rxv21/rxv21.h"
#include "rlv12/rlv12.h"
#include "bdv11/bdv11.h"
#include "dl11/dlv11j/dlv11j.h"
#include "msv11d/msv11d.h"
#include "ms11p/ms11p.h"
#include "kd/kd11_na/kd11_na.h"
#include "kd/kdf11/kdf11_a/kdf11_a.h"
#include "kd/kdf11/kdf11_b/kdf11_b.h"
#include "kd/kdf11/kdf11_u/kdf11_u.h"
#include "kt24/kt24.h"
#include "m9312/m9312.h"
#include "bus/include/bus.h"
#include "console/console.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "panel.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::unique_ptr;
using std::vector;

using namespace rxv21;

class PDP_11
{
	// A PDP-11 is using either a Unibus or a Qbus.
	Bus* bus_ {nullptr};

	// We would prefer using shared_ptr's as pointers to the devices but the
	// use of shared_ptr's makes debugging in MSVC terrribly slow. The use
	// of unique_ptr's for the decice pointers results in complicated code
	// and the use of raw pointers in e.g. Qbus::responsibleModule() as these
	// pointers are used in loadFile() and in run().
	//
	// The PDP11Processor is either a KD11_NA or a KDF11_A.
	PDP11Processor *processor_;
	vector<MemoryDevice*> memoryDevices_;
	vector<BusDevice*> busDevices_;
	BusDevice* kt24_ {nullptr};
	BusDevice* m9312_ {nullptr};
	unique_ptr<BA11_N> ba11_n_;
	unique_ptr<BA11_L> ba11_l_;
	CmdLineOptions const &cmdLineOptions_;

	void configureQbusSystem (SystemConfig const & systemConfig, Window* window);
	void configureUnibusSystem (SystemConfig const & systemConfig, Window* window);
	void installModules ();

public:
	PDP_11 (CmdLineOptions const &cmdLineOptions);
	~PDP_11 ();
	void configureDevices (Window *window);
	void configureDevices (SystemConfig const & systemConfig,
		Window *window);
	void run ();
	void reset ();
};

#endif // !_PDP11_H_
