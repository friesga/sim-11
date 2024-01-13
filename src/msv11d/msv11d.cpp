#include "types.h"
#include "msv11d.h"

#include <string.h>
#include <stdlib.h>

using std::bind;
using std::placeholders::_1;
using std::shared_ptr;

MSV11D::MSV11D (Qbus *bus)
	:
	PDP11Peripheral (bus),
	powerSource_ {MSV11Config::PowerSource::System},
	bank7Lower2kWEnabled_ {false}
{
	// Allocate and initialize the memory
	data = new u8[MSV11D_SIZE] ();

	// Subscribe to the BPOK signal
	bus_->BPOK().subscribe (bind (&MSV11D::BPOKReceiver, this, _1));
}

MSV11D::MSV11D (Qbus* bus, shared_ptr<MSV11Config> msv11Config)
	:
	MSV11D (bus)
{
	powerSource_ = msv11Config->powerSource;
	bank7Lower2kWEnabled_ = msv11Config->bank7Lower2kWEnabled;
}

MSV11D::~MSV11D ()
{
	delete[] data;
}

StatusCode MSV11D::read (BusAddress address, u16 *destAddress)
{
	// Get the contents of the specified address via a u16 pointer
	// as data is an array of bytes.
	u16* mem = (u16*) &data[address];
	*destAddress = *mem;
	return StatusCode::OK;
}

StatusCode MSV11D::writeWord (BusAddress address, u16 value)
{
	u16* mem = (u16*) &data[address];
	*mem = value;

	return StatusCode::OK;
}

bool MSV11D::responsible (BusAddress busAddress)
{
	bool addressIsInIOpage = (bank7Lower2kWEnabled_) ? 
		busAddress.isInIOpageLower2K () : 
		busAddress.isInIOpage ();

	return (!addressIsInIOpage && busAddress < MSV11D_SIZE) ? true : false;
}

void MSV11D::reset ()
{
	/* nothing */
}

// When the system is powered down the contents of the memory are lost, unless
// a battery backup is available. This means that when the power is switched
// on the memory is cleared when battery backup is not configured.
void MSV11D::BPOKReceiver (bool signalValue)
{
	if (signalValue && powerSource_ != MSV11Config::PowerSource::BatteryBackup)
		memset (data, 0, MSV11D_SIZE);
}