#ifndef _KT24_H_
#define _KT24_H_

#include "unibusmap.h"
#include "bus/bus.h"
#include "abstractbusdevice/abstractbusdevice.h"
#include "configdata/kt24/kt24config/kt24config.h"
#include "basicregister/twowordregister.h"
#include "interval.h"

#include <array>
#include <memory>

using std::array;
using std::shared_ptr;

// The KT24 is the Unibus map option for the PDP-11/24. It provides standard
// Unibus map functionality, extended with M9312-compatible functions and ROM
// sockets a power supply voltage monitor.
//
class KT24 : public UnibusMap
{
public:
	KT24 (Bus* bus);
	KT24 (Bus* bus, shared_ptr<KT24Config> kt24Config);

    // Functions required for the BusDevice interface
	StatusCode read (BusAddress address, u16* destination);
	StatusCode writeWord (BusAddress address, u16 value);
	StatusCode writeByte (BusAddress address, u8 value);
	bool responsible (BusAddress address);
	void reset ();

	// Functions required for the UnibusMap interface
	CondData<u16> dmaRead (BusAddress address);
	bool dmaWrite (BusAddress address, u16 value);

	// The enable() and disable() functions are added to be able to control
	// the KT24 state from the unit tests.
	void enable ();
	void disable ();

private:
    Bus* bus_;
	bool ioMapEnabled_ {false};

	// The KT24 Unibus option contains the Unibus map registers, the last
	// mapped address (LMA) register, and the CPU error register.
	enum KT24Registers : size_t
	{
        MappingRegister = 0,
        LMARegister = 1,
        CpuErrorRegister = 2,
		End = 3
	};

	// There are 32 mapping registers, each containing a 21-bit mapping
	// offset. Each mapping register is contained in two 16-bit registers;
	// one register contains bits (15:01) (low word), and the other register
	// contains bits (21:16) (high word).
	//
	// The mapping registers are defined as an array of u16 low/high pairs
	// to be easily indexed by bus address <17:13>.
	//
	static const size_t numMappingRegisters {32};
	static const u16 mappingRegistersBaseAddress {0170200};
	array<TwoWordRegister, numMappingRegisters> mappingRegisters_ {0};

	// The LMA is a two-word register located at addresses 017777734
	// (low word) and 017777736 (high word) that contains the 22-bit memory
	// (EUB) address of the last mapped address.This register is used for
	// maintenance purposes and also contains the memory control lines, C1
	// and C0, and the force jumpers bit.
	//
	static const u16 lmaRegisterBaseAddress {0177734};
	TwoWordRegister lmaRegister_ {0};

	// The CPU error register is located at address 017777766. This register
	// is read-only; any attempt to write to it will clear the power-fail bit.
	//
	static const u16 cpuErrorRegisterAddress {0177766};
	u16 cpuErrorRegister_ {0};

	static const Interval<u16> kt24Registers_[End];

	// Calculate the array index from the bus address. The division by four
	// stems from the facts that a mapping register comprises two words each
	// consisting of two bytes.
	constexpr size_t indexFromRegisterAddress (u16 address) const
		{ return (address - mappingRegistersBaseAddress) / 4; }
	constexpr size_t indexFrom18BitBusAddress (u32 address) const
		{ return (address & 0760000) >> 13; }
	constexpr bool isLowRegister (u16 address) const
		{ return (address % 4) == 0;}
	constexpr bool isHighRegister (u16 address) const
		{ return (address % 4) == 2; }

	KT24Registers findRegister (BusAddress address) const;
	void readMappingRegister (u16 registerAddress, u16* destination);
	void readLMARegister (u16 registerAddress, u16* destination);
	void writeMappingRegister (u16 registerAddress, u16 value);
	void writeLMARegister (u16 registerAddress, u16 value);
	void writeCpuErrorRegister ();
	CondData<u16> mappedRead (BusAddress address);
	bool mappedWrite (BusAddress address, u16 value);
	CondData<u16> readPhysical (u32 physicalAddress);
	bool writePhysical (u32 physicalAddress, u16 value);
	u32 physicalAddressFrom18BitBusAddress (BusAddress busAddress);
	void ioMapEnableReceiver (bool signalValue);
};

// The Interval's in this array must be in order of the KT24Registers enum
inline const Interval<u16> KT24::kt24Registers_[KT24::End] =
{
	{0170200, 0170400},		// MappingRegister
	{0177734, 0177740},		// LMARegister
	{0177766, 0177770}		// CpuErrorRegister
};

#endif // _KT24_H_