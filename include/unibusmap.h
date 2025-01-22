#ifndef _UNIBUSMAP_H_
#define _UNIBUSMAP_H_

#include "busdevice.h"
#include "conddata/conddata.h"

// PDP-11 systems which have a Unibus and more than 248 Kbytes of main memory
// are equipped with a Unibus map to allow DMA devices on the Unibus access to
// all of main memory. The Unibus map allows 8 Kbyte blocks of 18-bit Unibus
// address space to be mapped to 8 Kbyte blocks of main memory, located on any
// word boundary. The blocks of memory to which Unibus blocks are mapped are
// not restricted to be on 8 Kbyte boundaries themselves. The Unibus map in all
// PDP-11 systems has program compatible functionality.
//
// A Unibus map comprises 32 mapping registers  (of which 31 registers are
// actually used). A Unibus Map device therefore also is a BusDevice.
//
class UnibusMap : public BusDevice
{
public:
	// The LMA also contains the memory control lines C1 and C0
	static const u16 C1 {1 << 15};
	static const u16 C0 {1 << 14};

	virtual BusAddress physicalAddressFrom18bitAddress (BusAddress address) = 0;
	virtual void setControlLines (u16 controlLines) = 0;
	virtual ~UnibusMap () {};
};


#endif // _UNIBUSMAP_H_