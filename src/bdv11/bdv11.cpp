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

// Definition of the Switch Register lay-out.
// The switch register is used for maintenance and system configuration
// (selects diagnostic and/or bootstrap programs for execution). Bits 0-11
// of the register corresponding with switches E15-1 through E15-8 (hereafter
// named A1 through A8 and B1 through B8 respectively).
//
// Switches A 1 through B4 are defined as follows:
// A1 0N	Execute CPU tests on power-up or restart.
// A2 ON	Execute memory test on power-up or restart.
// A3 ON	DECNET BOOT - A4, A5, A6, A7 are used as arguments.
// A4 ON	Console test and dialog (A3 OFF).
// A4 OFF	Turnkey BOOT dispatched by switch setting (A3 OFF).
//			Switches A5, A6, A7, A8, B1 are used as arguments.
// 
//			Device			A5	A6	A7	A8	B1
//			Loop on Error	OFF OFF OFF OFF ON
//			RK05			OFF	OFF OFF ON	OFF
//			RL01			OFF OFF ON	OFF	OFF
//			RX01			OFF ON	OFF OFF OFF
//			RX02			OFF ON	ON	OFF OFF
//			BDV11 ROM		ON	OFF	OFF	OFF OFF
//
// Source: LSI-11 Systems Service Manual, 3rd Edition, module M8012.
//
#define	_A(x)		(1 << ((x) - 1))
#define	_B(x)		(1 << ((x) + 7))

#define	BDV11_CPU_TEST	_A(1)
#define	BDV11_MEM_TEST	_A(2)	
#define	BDV11_DECNET	_A(3)
#define	BDV11_DIALOG	_A(4)
#define	BDV11_LOOP	_B(1)
#define	BDV11_RK05	_A(8)
#define	BDV11_RL01	_A(7)
#define	BDV11_RX01	_A(6)
#define	BDV11_RX02	(_A(6) | _A(7))
#define	BDV11_ROM	_A(5)

#define	BDV11_EXT_DIAG	_B(2)
#define	BDV11_2780	_B(3)
#define	BDV11_PROG_ROM	_B(4)

#define	BDV11_SWITCH	(BDV11_CPU_TEST | BDV11_MEM_TEST \
		| BDV11_DIALOG | BDV11_RX02)

BDV11::BDV11 (Qbus *bus)
	:
	BusDevice (bus),
	pcr {0},
	scratch {0},
	option {0},
	display {0},
	ltc {0},
	time {0.0},
	ltcThread_ {thread(&BDV11::tick, this)},
	running_ {true}
{
	bus_->BINIT().subscribe (bind (&BDV11::BINITReceiver, this, _1));
	bus_->BPOK().subscribe (bind (&BDV11::BPOKReceiver, this, _1));
}

BDV11::~BDV11 ()
{
	running_ = false;
	ltcThread_.join();
}

u16 BDV11::getWordLow (u16 word)
{
	u16 page = pcr & 0xFF;
	if (page < 0x10)
	{
		u16 romword = page * 0200 + word;
		return bdv11_e53[romword];
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
		return bdv11_e53[romword];
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
			data = &bdv11_e53[romword];
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
			data = &bdv11_e53[romword];
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

StatusCode BDV11::read (u16 address, u16 *destAddress)
{
	switch (address) 
	{
		case 0177520:
			*destAddress = pcr;
			break;

		case 0177522:
			*destAddress = scratch;
			break;

		case 0177524:
			*destAddress = BDV11_SWITCH;
			break;

		case 0177546:
			// BEVNT register. According to the BDV11 technical manual
			// (EK-BDV11-TM-001) this is a write-only register.
			*destAddress = ltc;
			break;

		default:
			if (address >= 0173000 && address < 0173400)
			{
				*destAddress = getWordLow ((address - 0173000) / 2);
				break;
			} 
			else if (address >= 0173400 && address < 0173776) 
			{
				*destAddress = getWordHigh ((address - 0173400) / 2);
				break;
			}
			else
				return StatusCode::NonExistingMemory;
	}
	return StatusCode::OK;
}

StatusCode BDV11::writeWord (u16 address, u16 value)
{
	switch (address)
	{
		case 0177520:
			/* record new memory content in trace */
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

		case 0177522:
			scratch = value;
			break;

		case 0177524:
			display = value;
			break;

		case 0177546:
			// BEVNT register. Bit 6 controls the line time clock (LTC)
			// function.
			ltc = value & 0100;
			break;
	}

	return StatusCode::OK;
}

bool BDV11::responsible (u16 address)
{
	switch (address)
	{
		case 0177520:
		case 0177522:
		case 0177524:
		case 0177546:
			return true;
		default:
			return (address >= 0173000 && address <= 0173776) ? true : false;
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
		if (ltc & 0100)
		{
			bus_->setInterrupt (TrapPriority::BR4, 9, 0100);
			nextWakeup += cycleTime;
			this_thread::sleep_until (nextWakeup);
		}
	}
}
