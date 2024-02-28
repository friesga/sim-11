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

// Definition of the Switch Register lay-out.
// The switch register is used for maintenance and system configuration
// (selects diagnostic and/or bootstrap programs for execution). Bits 0-11
// of the register corresponding with switches E15-1 through E15-8 (hereafter
// named A1 through A8 and B1 through B8 respectively).
//
// Switches A1 through B4 are defined as follows:
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
consteval u16 A (size_t x)
{
    return 1 << (x - 1);
}

consteval u16 B (size_t x)
{
    return 1 << (x + 7);
}

static const u16 BDV11_CPU_TEST	 {A(1)};
static const u16 BDV11_MEM_TEST	 {A(2)};	
static const u16 BDV11_DECNET	 {A(3)};
static const u16 BDV11_DIALOG	 {A(4)};
static const u16 BDV11_LOOP		 {B(1)};
static const u16 BDV11_RK05		 {A(8)};
static const u16 BDV11_RL01		 {A(7)};
static const u16 BDV11_RX01		 {A(6)};
static const u16 BDV11_RX02		 {A(6) | A(7)};
static const u16 BDV11_ROM		 {A(5)};

static const u16 BDV11_EXT_DIAG	 {B(2)};
static const u16 BDV11_2780		 {B(3)};
static const u16 BDV11_PROG_ROM	 {B(4)};

// Definition of the default contents of the switch register in case no
// configuration options are specified
static const u16 BDV11_SWITCH	 {BDV11_CPU_TEST | BDV11_MEM_TEST |\
								  BDV11_DIALOG   | BDV11_RX02};

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
	switchRegister_ = 0;
	if (bdv11Config->cpuTests)
		switchRegister_ |= BDV11_CPU_TEST;
	if (bdv11Config->memoryTests)
		switchRegister_ |= BDV11_MEM_TEST;
	if (bdv11Config->decnetBoot)
		switchRegister_ |= BDV11_DECNET;
	if (bdv11Config->consoleDialog)
		switchRegister_ |= BDV11_DIALOG;
	
	switch (bdv11Config->bootDevice)
	{
		case BDV11Config::BootDevice::RK05:
			switchRegister_ |= BDV11_RK05;
			break;

		case BDV11Config::BootDevice::RL01:
			switchRegister_ |= BDV11_RL01;
			break;

		case BDV11Config::BootDevice::RX01:
			switchRegister_ |= BDV11_RX01;
			break;

		case BDV11Config::BootDevice::RX02:
			switchRegister_ |= BDV11_RX02;
			break;

		case BDV11Config::BootDevice::BDV11ROM:
			switchRegister_ |= BDV11_ROM;
			break;
	}
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
		return ROM_045E2_046E2[romword];
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
		return ROM_045E2_046E2[romword];
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
			data = &ROM_045E2_046E2[romword];
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
			data = &ROM_045E2_046E2[romword];
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

		case BER:
			// BEVNT Register. According to the BDV11 technical manual
			// (EK-BDV11-TM-001) this is a write-only register.
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

		case BER:
			// BEVNT register. Bit 6 controls the line time clock (LTC)
			// function.
			ltc = value & 0100;
			break;
	}

	return StatusCode::OK;
}

// As the BDV11 will only be accessed by means of unmapped (16-bit) addresses
// the given bus address can be compared directly with the BDV11's device
// addresses.
bool BDV11::responsible (BusAddress busAddress)
{
	// The BDV11 registers are word or byte addressable
	switch (busAddress.registerAddress () & 0177776)
	{
		case 0177520:
		case 0177522:
		case 0177524:
		case 0177546:
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
			bus_->setInterrupt (TrapPriority::BR4, 9, 0, 0100);
			nextWakeup += cycleTime;
			this_thread::sleep_until (nextWakeup);
		}
	}
}
