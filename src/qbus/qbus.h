#ifndef _QBUS_H_
#define _QBUS_H_

#include "types.h"
#include "statuscodes.h"
#include "conddata/conddata.h"
#include "interruptrequest/interruptrequest.h"
#include "threadsafecontainers/threadsafeprioqueue.h"
#include "pdp11peripheral/pdp11peripheral.h"
#include "signal/signal.h"

#include <string>
#include <array>
#include <functional>
#include <vector>

using std::string;
using std::array;
using std::function;
using std::vector;

// Qbus interrupt latency, defined as the maximum number of instructions
// after which the interrupt will be processed. The INTRPT_LATENCY_JITTER
// is a random number of instructions. The minimum latency will be
// INTRPT_LATENCY - INTRPT_LATENCY_JITTER and the maximum latency is the
// value of INTRPT_LATENCY.
// 
// According to the Microcomputer Processor Handbook, the interrupt
// latency lies between 35.05 and 44.1 +/- 20%, so roughly between 30 and
// 50 microseconds. The LSI-11 instruction time varies between 3.5
// and 15 microseconds.
// 
// At least a few instructions are necessary to satify the VKAAC0 and 
// VKADC1 diagnostics. An interrupt latency of 20 instructions will result
// in the VRLBC0 diagnostic reporting "NO INTERRUPT ON FUNCTION COMPLETE"
// errors. The XRLKB3 diagnostic expects an interrupt within five instructions
// after Controller Ready becomes true.
#define	INTRPT_LATENCY			5
#define	INTRPT_LATENCY_JITTER	2

class PDP11Peripheral;

// This class implements the Qbus backplane. Two variants of the backplane
// exist: 18-bit and 22-bit wide. As the 18-bit backplanes can be upgraded
// to a 22-bit without impact on the functionality of the system we implement
// the 22-bit version.
//
// Addresses on the Qbus are represented by a 32-bit integer. Devices in the
// I/O page ignore the nine high-order address bits BDAL<21:13> and instead
// decode BBS7 [which indicates an I/O page access] along with the thirteen
// low-order address bits. (Supersystems Handbook)
// 
// This means that register address for devices with registers in the I/O
// page can be limited to 16-bits. The 32-bit bus address will be truncated
// to a 16 bit address to be used by the device.
//
class Qbus
{
public:
	static const size_t numberOfSlots {9};

private:
	// A BA11-N backplane has nine slots, named ROW 1 to ROW 9. ROW 1 corresponds
	// with slot[0], ROW 9 with slot [8].
	size_t numDevices_ {0};
	BusDevice* slots_[numberOfSlots] {nullptr};

public:
	// The bus contains a number of BusDevice pointers. This iterator iterates
	// over these pointers. The iterator therefore points to BusDevice pointers.
	class Iterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type   = std::ptrdiff_t;
		using value_type        = BusDevice*;
		using pointer           = BusDevice**;  // or also value_type*
		using reference         = const value_type&;

		// In the default constructor initialize the iterator with a default
		// null pointer.
		Iterator () : ptr_ {nullptr} {}
		Iterator (pointer it) : ptr_ (it) {}
		reference operator* () const { return *ptr_; }
		pointer operator-> () { return &(*ptr_); }

		// Prefix increment
		Iterator& operator++ () { ++ptr_; return *this; }

		// Postfix increment
		Iterator operator++ (int) { Iterator tmp = *this; ++(*this); return tmp; }

		friend bool operator== (const Iterator& a, const Iterator& b) { return a.ptr_ == b.ptr_; }
		friend bool operator!= (const Iterator& a, const Iterator& b) { return a.ptr_ != b.ptr_; }

	private:
		pointer ptr_;
	};

	Iterator begin () { return Iterator (&slots_[0]); }
	Iterator end () { return Iterator (&slots_[numDevices_ - 1]); }
	Iterator operator[] (int index) { return Iterator (&slots_[index]); }

	// The Qbus contains a set of control signals that can be set, cycled
	// and tested.
	//
	// The SRUN L signal is actually not a bus signal, but a non-bused, backplane
	// signal. The signal is a series of pulses which occur at 3-5" intervals
	// whenever the processor is in the Run mode.
	//
	// The BPOK signal is triggered:
	// - On power up or,
	// - When the console BREAK key is hit and the boot response is configured.
	// On assertion of this signal the processor executes the power up routine.
	// 
	// The PDP Bus Handbook states that "BOCOK H may be pulsed low for a minimum
	// of 1 microsecond to cause the CPU to restart" while BPOK is high. This
	// pulse of BDCOK is replaced by a separate RESET signal.
	// 
	// The BINIT signal is triggered:
	// - When the processor executes the power up routine,
	// - On a RESET instruction,
	// - On a ODT Go command.
	// On assertion of this signal all bus devices - including the bus itself -
	// are initialized.
	// 
	// These signals are (or will become) abstraction of the actual bus
	// Unibus or Qbus signals and not every system will contain all of these
	// signals. These signals are generic and a system can take use of it to
	// provide specific functionality. The 11/24 e.g. uses the BOOT signal
	// which is not used by the 11/03 and 11/23 systems.
	//
	Signal& SRUN ();
	Signal& BPOK ();
	Signal& RESET ();
	Signal& BHALT ();
	Signal& BINIT ();
	Signal& BOOT ();
	Signal& BatteryPower ();

	Qbus ();
	void setInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder, unsigned char vector);
	bool containsInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder);
	void clearInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder);
	void clearInterrupts ();
	bool intrptReqAvailable ();
	u8 intrptPriority ();
	bool getIntrptReq (InterruptRequest &ir);

	void step ();
	template <ValidBusAddress T> CondData<u16> read (T address);
	template <ValidBusAddress T> bool writeWord (T address, u16 value);
	template <ValidBusAddress T> bool writeByte (T, u8 val);
	bool installModuleAtPosition (BusDevice* module, size_t position);
	bool installModule (BusDevice* module);

private:
	// This queue keeps all interrupt requests, ordered in interrupt priority
	using IntrptReqQueue = ThreadSafePrioQueue<InterruptRequest>;
	IntrptReqQueue intrptReqQueue_;

	// Definition of the bus signals. The BatteryPower signal is present
	// only on systems with battery backup. The initial value of the
	// BatteryPower signal is false and in systems without battery backup
	// that signal remains false.
	Signal SRUN_;
	Signal BPOK_;
	Signal RESET_;
	Signal BHALT_;
	Signal BINIT_;
	Signal BOOT_;
	Signal BatteryPower_;

	// Signal administration
	Signal::SubscriberKey ourKey_;

	bool processorRunning_;
	u16	delay_;

	void reset ();
	template <ValidBusAddress T> BusDevice* responsibleModule (T address);
	void pushInterruptRequest (InterruptRequest interruptReq);
	void BINITReceiver (bool signalValue);
};

// Explicit template instantations
template CondData<u16> Qbus::read<BusAddress<16>> (BusAddress<16> address);
template CondData<u16> Qbus::read<BusAddress<18>> (BusAddress<18> address);
template CondData<u16> Qbus::read<BusAddress<22>> (BusAddress<22> address);

template bool Qbus::writeWord<BusAddress<16>> (BusAddress<16> address, u16 value);
template bool Qbus::writeWord<BusAddress<18>> (BusAddress<18> address, u16 value);
template bool Qbus::writeWord<BusAddress<22>> (BusAddress<22> address, u16 value);

template bool Qbus::writeByte<BusAddress<16>> (BusAddress<16> address, u8 val);
template bool Qbus::writeByte<BusAddress<18>> (BusAddress<18> address, u8 val);
template bool Qbus::writeByte<BusAddress<22>> (BusAddress<22> address, u8 val);

template BusDevice* Qbus::responsibleModule<BusAddress<16>> (BusAddress<16> address);
template BusDevice* Qbus::responsibleModule<BusAddress<18>> (BusAddress<18> address);
template BusDevice* Qbus::responsibleModule<BusAddress<22>> (BusAddress<22> address);

#endif // !_QBUS_H_