#include "ms11p.h"

using std::shared_ptr;

MS11P::MS11P (Qbus* bus)
	:
	PDP11Peripheral (bus)
{
	// Allocate zero-initialized memory
	memory_ = make_unique<u8[]> (memorySize_);
}

MS11P::MS11P (Qbus* bus, shared_ptr<MS11PConfig> ms11pConfig)
	:
	MS11P (bus)
{
	powerSource_ = ms11pConfig->powerSource;
	startingAddress_ = ms11pConfig->startingAddress;
	csrAddress_ = ms11pConfig->csrAddress;
}

MS11P::~MS11P ()
{}

StatusCode MS11P::read (BusAddress address, u16* destAddress)
{
	// Get the contents of the specified address via a u16 pointer
	// as data is an array of bytes.
	u16* mem = (u16*)&memory_[address - startingAddress_];
	*destAddress = *mem;
	return StatusCode::OK;
}

StatusCode MS11P::writeWord (BusAddress address, u16 value)
{
	u16* mem = (u16*)&memory_[address - startingAddress_];
	*mem = value;

	return StatusCode::OK;
}

bool MS11P::responsible (BusAddress address)
{
	return address >= startingAddress_ &&
		address < startingAddress_ + memorySize_;
}

void MS11P::reset ()
{}