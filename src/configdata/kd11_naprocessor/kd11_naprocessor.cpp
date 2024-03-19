#include "kd11_naprocessor.h"
#include "../kd11_naconfig/kd11_naconfig.h"

using std::make_unique;
using std::move;
using std::invalid_argument;

// To be able to pass the unique_ptr<KD11_NAConfig> as a KD11Config pointer
// to KD11Processor, we have to pass the unique_ptr to KD11Processor via the
// init() function as KD11Processor's constructor is called before the
// unique_ptr is initialized.
KD11_NAProcessor::KD11_NAProcessor ()
	:
	KD11Processor (),
	kd11ConfigPtr {make_unique<KD11_NAConfig> ()}
{
	init (kd11ConfigPtr.get ());
}


DeviceConfigVariant KD11_NAProcessor::getConfig ()
{
	return move (kd11ConfigPtr);
}