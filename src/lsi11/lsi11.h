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

class LSI11
{
	Qbus	 bus;
	// We would prefer using shared_ptr's as pointers to the devices but the
	// use of shared_ptr's makes debugging in MSVC terrribly slow. The use
	// of unique_ptr's for the decice pointers results in complicated code
	// and the use of raw pointers in e.g. Qbus::responsibleModule() as these
	// pointers are used in loadFile() and in run().
	KD11 *kdv11_;
	MSV11D *msv11_;
	RXV21 *rxv21_;
	RLV12 *rlv12_;
	DLV11J *dlv11_;
	BDV11 *bdv11_;
	unique_ptr<BA11_N> ba11_n_;
	unique_ptr<Console> console_;
	CmdLineOptions const &cmdLineOptions_;
	bool running_;

	// RX02 double density bootstrap
	const char* odt_input =
		"2000/012701\r"
		"2002/177170\r"
		"2004/012700\r"
		"2006/100240\r"
		"2010/005002\r"
		"2012/012705\r"
		"2014/000200\r"
		"2016/012704\r"
		"2020/000401\r"
		"2022/012703\r"
		"2024/177172\r"
		"2026/030011\r"
		"2030/001776\r"
		"2032/100440\r"
		"2034/012711\r"
		"2036/000407\r"
		"2040/030011\r"
		"2042/001776\r"
		"2044/100433\r"
		"2046/110413\r"
		"2050/000304\r"
		"2052/030011\r"
		"2054/001776\r"
		"2056/110413\r"
		"2060/000304\r"
		"2062/030011\r"
		"2064/001776\r"
		"2066/100422\r"
		"2070/012711\r"
		"2072/000403\r"
		"2074/030011\r"
		"2076/001776\r"
		"2100/100415\r"
		"2102/010513\r"
		"2104/030011\r"
		"2106/001776\r"
		"2110/100411\r"
		"2112/010213\r"
		"2114/060502\r"
		"2116/060502\r"
		"2120/122424\r"
		"2122/120427\r"
		"2124/000007\r"
		"2126/003737\r"
		"2130/005000\r"
		"2132/005007\r"
		"2134/000000\r"
		"2000G";

	void checkConsistency (vector<shared_ptr<DeviceConfig>> systemConfig);
	void loadFile ();

public:
	LSI11 (CmdLineOptions const &cmdLineOptions);
	~LSI11 ();
	void configureDevices ();
	void run ();
	void reset ();
	void step ();
};

#endif // !_LSI11_H_
