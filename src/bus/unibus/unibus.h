#ifndef _UNIBUS_H_
#define _UNIBUS_H_

#include "bus/include/bus.h"
#include "bus/signalhandler/signalhandler.h"
#include "bus/interrupthandler/interrupthandler.h"
#include "bus/configurationhandler/configurationhandler.h"
#include "bus/iterator/iterator.h"
#include "unibusmap.h"

// This class implements both the Unibus and the Extended Unibus (EUB).
// 
// Addresses on the Unibus are 18-bits wide while addresses on the EUB are
// 22-bits. The (optional) Unibus Map maps 18-bit addresses to 22-bit
// adresses destined for the EUB.
// 
// In a PDP-11 system with the Unibus Map, when the processor generates
// a 22-bit physical address through the MMU, the upper address bits
// (bits 18�21) determine whether the address is routed to the Unibus.
// If these bits are set (non-zero), the address is interpreted as targeting
// the Unibus. The lower 18 bits (bits 0�17) are then used as the Unibus
// address, and the Unibus Map is involved in translating this 18-bit Unibus
// address to the actual 22-bit memory address.
//
// Key Points :
//
// 1. Processor Accesses via MMU :
//    - The processor generates a 22-bit physical address using the MMU.
//    - If the address falls within the range where bits 18�21 are non-zero,
//      it is routed to the Unibus.
//    - The lower 18 bits are treated as the Unibus address.
//
// 2. Unibus Map Role :
//    - The Unibus Map translates the 18-bit Unibus address (from bits 0�17
//      of the processor-generated address) into a full 22-bit main memory
//      address.
//    - This enables peripherals on the Unibus to access physical memory
//      locations beyond the 18-bit (256 KB) limit of the Unibus itself.
//
// Diagnostic CKKUAE0 contains in paragraph 8.2 an illustrative text on
// the mapping proces which is reproduced below.
// ----
// The following is an example of how a memory address is generated by the
// Unibus Map. This assumes that the address originates in the CPU but the
// process can apply to any Unibus address, starting at line C2.
// 
// A.  Virtual Address                            15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// A1. PAR Page Number (0-7)                      15 14 13
// A2. Offset (from virtual address)                       12 11 10 09 08 07 06 05 04 03 02 01 00
// B.  PAR [Page No.]           15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
//                            + -----------------------------------------------------------------
// C.  Phys Address (A2+B)      21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// 
// C1. 17XXXXXX => U.B. Addr    21 20 19 18
// C2. Mapping reg. No. (0-36)              17 16 15 14 13
// C3. Offset                                              12 11 10 09 08 07 06 05 04 03 02 01 00
// D. Map Reg[No.]              21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01
//                            + -----------------------------------------------------------------
// E. Phys Addr (C3+D)          21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
//
// Description of lines:
// A:  Virtual Address (16 bits)
// A1: Upper 3 bits of virtual address, used to select a Page Address Register (PAR)
// A2: Lower 13 bits of virtual address, added to [contents of] selected PAR
// 
// B:  Page Address Register (16 bits), in addition process this gets left shifted 6 bits
//     before addition to A2.
// 
// C:  Physical address created by memory management (22 bits)
// C1: If upper 4 bits are allo nes then bits <17:00> go out on Unibus
// C2: If map relocation is enabled then bits <17:23> select one of the 36 (octal)
//     map registers.
// C3: Lower 13 bits of Unibus Address, added to [contents of] selected map register
// 
// D:  Map register (22 bits), added to bits <12:00> of Unibus address
// 
// E:  Physical address generated by Unibus map and sent to the cache.
// ----
//
class Unibus : public Bus
{
public:
    Unibus();

	// Functions required for the BusSignals interface
	Signal& SRUN ();
	Signal& BPOK ();
	Signal& RESET ();
	Signal& BHALT ();
	Signal& BINIT ();
	Signal& BOOT ();
	Signal& BatteryPower ();
	Signal& IOMapEnable ();

	// Functions required for the BusInterrupts interface
	void setInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder, unsigned char vector);
	bool containsInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder);
	void clearInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder);
	void clearInterrupts ();
	bool intrptReqAvailable ();
	u8 intrptPriority ();
	bool getIntrptReq (InterruptRequest& ir);

	// Functions required for the BusConfiguration interface
	bool installModuleAtPosition (BusDevice* module, size_t position);
	bool installModule (BusDevice* module);
	BusDevice* responsibleModule (BusAddress address);
	Iterator begin ();
	Iterator end ();
	size_t capacity ();
	Iterator operator[] (int index);
	void reset ();

	// Functions required for the BusDataTransfer interface
	CondData<u16> read (BusAddress address);
	bool writeWord (BusAddress address, u16 value);
	bool writeByte (BusAddress address, u8 val);
	CondData<u16> dmaRead (BusAddress address);
	bool dmaWrite (BusAddress address, u16 value);

private:
	// Definition of the handlers
	SignalHandler signalHandler_;
	InterruptHandler interruptHandler_;
	ConfigurationHandler configurationHandler_;

	// Signal administration
	Signal::SubscriberKey ourKey_;
	void BINITReceiver (bool signalValue);

	// A Unibus optionally contains a Unibus Map
	UnibusMap* unibusMap_;

	bool addressMustBeMapped (BusAddress address);
	CondData<u16> mappedRead (BusAddress address);
	CondData<u16> physicalRead (BusAddress address);
};


#endif // _UNIBUS_H_