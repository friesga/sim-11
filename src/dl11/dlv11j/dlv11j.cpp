#include "trace/trace.h"
#include "dlv11j.h"

#include <string.h>
#include <stdlib.h>
#include <functional>
#include <ranges>

using std::bind;
using std::placeholders::_1;
using std::make_unique;
using std::ranges::any_of;

// Construct a DLV11-J object with the given configuration
DLV11J::DLV11J (Qbus *bus, shared_ptr<DLConfig> dlConfig)
	:
	PDP11Peripheral (bus),
	baseAddress_ {dlConfig->baseAddress},
	baseVector_ {dlConfig->baseVector},
	dlConfig_ {dlConfig}
{
	initialize ();

	reset ();
}

// Initialize the DLV11J and its channels
// 
// If channel 3 is to be used as console the channel's base address
// and vector have to be set to the appropriate values.
//
// When channel 3 is configured as the console device interface [...]
// the interrupt vectors of the channel become 60 and 64. This is true
// regardless of the configured base vector of the module.
// (EK-DLV1J-UG-001)
//
void DLV11J::initialize ()
{
	for (u16 channelNr = 0; channelNr < numChannels; ++channelNr)
	{
		channel_[channelNr] = make_unique<DLV11Channel> (bus_,
				dlConfig_->uarts[channelNr], channelNr, dlConfig_);
	}

	bus_->BINIT().subscribe (bind (&DLV11J::BINITReceiver, this, _1));
}

// This function allows the host system to read a word from one of the
// DLV11-J's registers. The read command is passed through to the appropriate
// channel.
StatusCode DLV11J::read (BusAddress busAddress, u16 *destAddress)
{
	return channel_[extractChannelNr (busAddress)]->read (busAddress,
		destAddress);
}

// This function allows the host system to write a word to one of the
// DLV11-J's registers. The write command is passed through to the appropriate
// channel.
StatusCode DLV11J::writeWord (BusAddress busAddress, u16 value)
{
	return channel_[extractChannelNr (busAddress)]->writeWord (busAddress,
		value);
}

// The responsibility for the handling of bus addresses is delegated to the
// DLV11-J's channels.
bool DLV11J::responsible (BusAddress busAddress)
{
	if (!busAddress.isInIOpage ())
		return false;

	auto isResponsible = [busAddress] (unique_ptr<DLV11Channel> &channel)
		{return channel->responsible (busAddress); };

	return any_of (channel_, isResponsible);
}

// On assertion of the BINIT signal initialize the device.
void DLV11J::BINITReceiver (bool signalValue)
{
	if (signalValue)
		reset ();
}

void DLV11J::reset ()
{
	for (u8 channelNr = 0; channelNr < numChannels; ++channelNr)
		channel_[channelNr]->reset ();
}

// Determine the channel number from the register address. An exception
// to the standard formula has to be made for the use of channel 3 as
// console device.
u16 DLV11J::extractChannelNr (BusAddress busAddress)
{
	if (busAddress.registerAddress () >= defaultCh3Address_ && 
			busAddress.registerAddress () < defaultCh3Address_ + (3 * 8))
		return 3;
	else
		return static_cast<u16> ((busAddress.registerAddress  () & 030) >> 3);
}

