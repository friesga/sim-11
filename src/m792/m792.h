#ifndef _M792_H_
#define _M792_H_

#include "bus/include/bus.h"
#include "abstractbusdevice/abstractbusdevice.h"
#include "configdata/m792/m792config/m792config.h"

#include <array>

using std::array;

// The BM792 [mostly designated as M792] is a 32-word read-only-memory (ROM).
// The ROM is available either unprogrammed (designated BM792) or
// preprogrammed (designated BM792-Y X, where the letter in the X position
// identifies the program). The unprogrammed module can be programmed to form
// code conversion tables or contain frequently-used mathematical values and
// subroutines.
// 
// Preprogrammed ROMs are used for implementing small standard programs
// required in PDP-Il System operation, such as bootstrap loaders for paper
// tape or DECtape.
// 
// Source: BM792 read-only-memory and MR11-DB bootstrap loader
// (DEC-11-HBMAA-E-D).
//
// Currently the only supported preprogrammed version is the M792-YB, which
// contains the bulk storage bootstrap loader.
//
class M792 : public AbstractBusDevice
{
public:
	M792 (Bus* bus, const M792Config& m792Config);

    // Functions required by the AbstractBusDevice
	CondData<u16> read (BusAddress address) override;
	StatusCode writeWord (BusAddress address, u16 value) override;
	bool responsible (BusAddress address) override;
	void reset () override;

private:
	const u16 addressLow {0173100};
	const u16 addressHigh {0173177};

	static constexpr std::size_t m792ROMSize = 32;
	using m792ROMImage = array<u16, m792ROMSize> const;

	static const m792ROMImage rom_M792_YB;
};

#endif // _M792_H_