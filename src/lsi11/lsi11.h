#ifndef _LSI11_H_
#define _LSI11_H_

#include "kd/include/pdp11processor.h"
#include "ba11_n/ba11_n.h"
#include "rxv21/rxv21.h"
#include "rlv12/rlv12.h"
#include "bdv11/bdv11.h"
#include "dl11/dlv11j/dlv11j.h"
#include "msv11d/msv11d.h"
#include "kd/kd11_na/kd11_na.h"
#include "kd/kdf11/kdf11_a/kdf11_a.h"
#include "kd/kdf11/kdf11_b/kdf11_b.h"
#include "qbus/qbus.h"
#include "console/console.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "panel.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::unique_ptr;
using std::vector;

using namespace rxv21;

class LSI11
{
	Qbus bus_;
	// We would prefer using shared_ptr's as pointers to the devices but the
	// use of shared_ptr's makes debugging in MSVC terrribly slow. The use
	// of unique_ptr's for the decice pointers results in complicated code
	// and the use of raw pointers in e.g. Qbus::responsibleModule() as these
	// pointers are used in loadFile() and in run().
	//
	// The PDP11Processor is either a KD11_NA or a KDF11_A.
	PDP11Processor *processor_;
	vector<MSV11D*> msv11_;
	RXV21 *rxv21_;
	RLV12 *rlv12_;
	DLV11J *dlv11_;
	BDV11 *bdv11_;
	unique_ptr<BA11_N> ba11_n_;
	CmdLineOptions const &cmdLineOptions_;

	void checkConsistency (vector<shared_ptr<DeviceConfig>> systemConfig);
	u16 loadFile ();
	void installModules ();

public:
	LSI11 (CmdLineOptions const &cmdLineOptions);
	~LSI11 ();
	void configureDevices (Window *window);
	void configureDevices (vector<shared_ptr<DeviceConfig>> systemConfig,
		Window *window);
	void run ();
	void reset ();
};

#endif // !_LSI11_H_
