#ifndef _LSI11_H_
#define _LSI11_H_

#include "ba11_n/ba11_n.h"
#include "rxv21/rxv21.h"
#include "rlv12/rlv12.h"
#include "bdv11/bdv11.h"
#include "dlv11j/dlv11j.h"
#include "msv11d/msv11d.h"
#include "kd11/kd11.h"
#include "qbus/qbus.h"
#include "console/console.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <memory>

using std::shared_ptr;
using std::unique_ptr;

using namespace rxv21;

class LSI11
{
	Qbus bus_;
	// We would prefer using shared_ptr's as pointers to the devices but the
	// use of shared_ptr's makes debugging in MSVC terrribly slow. The use
	// of unique_ptr's for the decice pointers results in complicated code
	// and the use of raw pointers in e.g. Qbus::responsibleModule() as these
	// pointers are used in loadFile() and in run().
	KD11 *kd11_;
	MSV11D *msv11_;
	RXV21 *rxv21_;
	RLV12 *rlv12_;
	DLV11J *dlv11_;
	BDV11 *bdv11_;
	unique_ptr<BA11_N> ba11_n_;
	CmdLineOptions const &cmdLineOptions_;

	void checkConsistency (vector<shared_ptr<DeviceConfig>> systemConfig);
	void loadFile ();
	void installModules ();

public:
	LSI11 (CmdLineOptions const &cmdLineOptions);
	~LSI11 ();
	void configureDevices ();
	void configureDevices (vector<shared_ptr<DeviceConfig>> systemConfig);
	void run ();
	void reset ();
};

#endif // !_LSI11_H_
