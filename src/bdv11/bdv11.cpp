#include "trace/trace.h"
#include "bdv11.h"

#include <string.h>
#include <stdlib.h>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;
using std::bind;
using std::placeholders::_1;
using std::shared_ptr;

BDV11::BDV11 (Qbus *bus)
	:
	PDP11Peripheral (bus),
	pcr {0},
	scratch {0},
	option {0},
	display {0},
	ltc {0},
	switchRegister_ {BDV11_SWITCH},
	time {0.0},
	ltcThread_ {thread(&BDV11::tick, this)},
	running_ {true}
{
	bus_->BINIT().subscribe (bind (&BDV11::BINITReceiver, this, _1));
	bus_->BPOK().subscribe (bind (&BDV11::BPOKReceiver, this, _1));
}


BDV11::BDV11 (Qbus *bus, shared_ptr<BDV11Config> bdv11Config)
	:
	BDV11 (bus)
{
	switchRegister_ = switchRegisterProgramSelection (bdv11Config);
	switchRegister_ |= switchRegisterBootDevice (bdv11Config);

	romUsed_ = romToUse (bdv11Config);
}

BDV11::~BDV11 ()
{
	running_ = false;
	ltcThread_.join();
}

u16 BDV11::switchRegisterProgramSelection (shared_ptr<BDV11Config> bdv11Config)
{
	u16 switchRegister {0};

	if (bdv11Config->cpuTests)
		switchRegister |= BDV11_CPU_TEST;
	if (bdv11Config->memoryTests)
		switchRegister |= BDV11_MEM_TEST;
	if (bdv11Config->decnetBoot)
		switchRegister |= BDV11_DECNET;
	if (bdv11Config->consoleDialog)
		switchRegister |= BDV11_DIALOG;

	return switchRegister;
}

u16 BDV11::switchRegisterBootDevice (shared_ptr<BDV11Config> bdv11Config)
{
	return bootDevices.find (bdv11Config->bootDevice)->second;
}

BDV11::ROMimage* BDV11::romToUse (shared_ptr<BDV11Config> bdv11Config)
{
	return &availableROMs.find (bdv11Config->bootROM)->second;
}

u16 BDV11::getWordLow (u16 word)
{
	u16 page = pcr & 0xFF;
	if (page < 0x10)
	{
		u16 romword = page * 0200 + word;
		return (*romUsed_)[romword];
	}
	else
	{
		return 0177777;
	}
}

u16 BDV11::getWordHigh (u16 word)
{
	u16 page = (pcr >> 8) & 0xFF;
	if (page < 0x10)
	{
		u16 romword = page * 0200 + word;
		return (*romUsed_)[romword];
	} 
	else 
	{
		return 0233;
	}
}

void BDV11::memoryDump (u16 pcr, int hi)
{
	const u16* data;
	u16 addr;
	u16 len = 0400;
	if (hi)
	{
		addr = 0173400;
		u16 page = (pcr >> 8) & 0xFF;
		if (page < 0x10) 
		{
			u16 romword = page * 0200;
			data = &(*romUsed_)[romword];
		} 
		else 
		{
			data = NULL;
		}
	} 
	else 
	{
		addr = 0173000;
		u16 page = pcr & 0xFF;
		if (page < 0x10) 
		{
			u16 romword = page * 0200;
			data = &(*romUsed_)[romword];
		} 
		else 
		{
			data = NULL;
		}
	}

	if (data) 
		trace.memoryDump ((u8*) data, addr, len);
	else 
	{
		u8 buf[0400];
		memset (buf, 0xFF, sizeof(buf));
		trace.memoryDump ((u8*) data, addr, len);
	}
}

StatusCode BDV11::read (BusAddress busAddress, u16 *destAddress)
{
	switch (busAddress.registerAddress ()) 
	{
		case PCR:
			// Page Control Register
			*destAddress = pcr;
			break;

		case RWR:
			// Read/Write Register
			*destAddress = scratch;
			break;

		case SDR:
			// Switch and Display Register
			*destAddress = switchRegister_;
			break;

		case LKS:
			// Line Clock Status Register. According to the BDV11 technical
			// manual (EK-BDV11-TM-001) this is a write-only register.
			*destAddress = ltc;
			break;

		default:
			if (busAddress.registerAddress () >= 0173000 && 
				busAddress.registerAddress () < 0173400)
			{
				*destAddress = getWordLow ((busAddress.registerAddress () - 0173000) / 2);
				break;
			} 
			else if (busAddress.registerAddress () >= 0173400 &&
				busAddress.registerAddress () <= 0173776) 
			{
				*destAddress = getWordHigh ((busAddress.registerAddress () - 0173400) / 2);
				break;
			}
			else
				return StatusCode::NonExistingMemory;
	}
	return StatusCode::OK;
}

StatusCode BDV11::writeWord (BusAddress busAddress, u16 value)
{
	switch (busAddress.registerAddress ())
	{
		case PCR:
			// Page Control Register
			// Record new memory content in trace */
			if (value != pcr)
			{
				if ((value & 0xFF) == (pcr & 0xFF)) 
				{
					memoryDump (value, 1);
				} 
				else if ((value & 0xFF00) == (pcr & 0xFF00)) 
				{
					memoryDump (value, 0);
				} 
				else 
				{
					memoryDump (value, 0);
					memoryDump (value, 1);
				}
			}
			pcr = value;
			break;

		case RWR:
			// Read/Write Register
			scratch = value;
			break;

		case SDR:
			// Switch and Display Register
			display = value;
			break;

		case LKS:
			// Line Clock Status register
			writeLKS (value);
			break;
	}

	return StatusCode::OK;
}

// Write the Line Time Clock Status Register. Clearing the Interrupt Enable
// bit does not imply that a pending interrupt has to be cleared. This
// behaviour is not documented but is implied by diagnostic JKDIB0 test 50.
void BDV11::writeLKS (u16 value)
{
	ltc = value & LKS_IE;
}

// As the BDV11 will only be accessed by means of unmapped (16-bit) addresses
// the given bus address can be compared directly with the BDV11's device
// addresses.
bool BDV11::responsible (BusAddress busAddress)
{
	// The BDV11 registers are word or byte addressable
	switch (busAddress.registerAddress () & 0177776)
	{
		case PCR:
			// Page Control Register
		case RWR:
			// Read/Write Register
		case SDR:
			// Switch and Display Register
		case LKS:
			// Line Clock Status Register
			return true;
		default:
			return (busAddress.registerAddress () >= 0173000 && 
				busAddress.registerAddress () <= 0173776) ? true : false;
	}
}

// On assertion of the BINIT signal initialize the device.
void BDV11::BINITReceiver (bool signalValue)
{
	if (signalValue)
		reset ();
}

// On power down switch off the line time clock.
void BDV11::BPOKReceiver (bool signalValue)
{
	if (!signalValue)
		ltc = 0;
}

void BDV11::reset ()
{
	pcr = 0;
	scratch = 0;
	display = 0;
	ltc = 0;

	// Clear possibly pending interrupts
	bus_->clearInterrupt (TrapPriority::BR6, 9, 0);

	memoryDump (pcr, 0);
	memoryDump (pcr, 1);
}

// Generate an Event interrupt on every clock tick To get accurate timing
// the high_resolution_clock is used with an interval specified in
// nanoseconds.
void BDV11::tick()
{
	high_resolution_clock::time_point nextWakeup {high_resolution_clock::now()};
	constexpr duration<int, std::nano> cycleTime {nanoseconds (1000000000 / LTC_RATE)};

	while (running_)
	{
		// Check the line time clock (LTC) is enabled
		if (ltc & LKS_IE)
		{
			bus_->setInterrupt (TrapPriority::BR6, 9, 0, 0100);
			nextWakeup += cycleTime;
			this_thread::sleep_until (nextWakeup);
		}
	}
}
