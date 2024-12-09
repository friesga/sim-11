#ifndef _M9312_H_
#define _M9312_H_

#include "pdp11peripheral/pdp11peripheral.h"
#include "configdata/m9312/m9312config/m9312config.h"
#include "statuscodes.h"
#include "unary_operator_plus.h"

#include <memory>
#include <array>

using std::shared_ptr;
using std::array;

// The M9312 Bootstrap/Terminator Module contains a complete set of Unibus
// termination resistors along with 512 words of read only memory (ROM) that
// can be used for diagnostic routines, the console emulator routine, and
// bootstrap programs.
// 
// Five sockets on the M9312 allow the user to interchange ROMs, enabling
// the module to be used with any Unibus PDP-11 system and boot any peripheral
// device by simply adding or changing ROMs. One socket is solely used for a
// diagnostic ROM (PDP-11/60 and 11/70 systems) or a ROM which contains the
// console emulator routine and diagnostics for all other PDP-11 systems.The
// other four sockets accept ROMs which contain bootstrap programs.
//
// Diagnostics, bootstrap programs, and the console emulator routine are all
// selectable through the Address Offset Switch Bank on the M9312.
//
class M9312 : public PDP11Peripheral
{
public:
    M9312 (Qbus* bus, shared_ptr<M9312Config> m9312Config);

    // Functions required by the BusDevice interface
    void reset () override;
    bool responsible (BusAddress addr) override;
    StatusCode read (BusAddress busAddress, u16* data) override;
    StatusCode writeByte (BusAddress busAddress, u8 data) override;
    StatusCode writeWord (BusAddress busAddress, u16 data) override;

private:
    using DiagROMImage = array<u16, 256> const;
    using BootROMImage = array<u16, 64> const;

    static const DiagROMImage rom_23_248F1;
    static const DiagROMImage rom_23_446F1;
    static const DiagROMImage rom_23_616F1;
    static const DiagROMImage rom_23_774F1;

    static const BootROMImage rom_23_751A9;
    static const BootROMImage rom_23_752A9;
    static const BootROMImage rom_23_753A9;
    static const BootROMImage rom_23_755A9;
    static const BootROMImage rom_23_756A9;
    static const BootROMImage rom_23_757A9;
    static const BootROMImage rom_23_758A9;
    static const BootROMImage rom_23_759A9;
    static const BootROMImage rom_23_760A9;
    static const BootROMImage rom_23_761A9;
    static const BootROMImage rom_23_764A9;
    static const BootROMImage rom_23_765A9;
    static const BootROMImage rom_23_767A9;
    static const BootROMImage rom_23_811A9;
    static const BootROMImage rom_23_862A9;
    static const BootROMImage rom_23_863A9;
    static const BootROMImage rom_23_864A9;
    static const BootROMImage rom_23_E32A9;
    static const BootROMImage rom_23_E33A9;
    static const BootROMImage rom_23_E39A9;

    array<DiagROMImage*, +M9312Config::DiagROMType::NUMBER> diagROMImages_ =
    {
        &rom_23_248F1,
        &rom_23_446F1,
        &rom_23_616F1,
        &rom_23_774F1
    };

    array<BootROMImage*, +M9312Config::BootROMType::NUMBER> bootROMImages_ =
    {
        &rom_23_751A9,
        &rom_23_752A9,
        &rom_23_753A9,
        &rom_23_755A9,
        &rom_23_756A9,
        &rom_23_757A9,
        &rom_23_758A9,
        &rom_23_759A9,
        &rom_23_760A9,
        &rom_23_761A9,
        &rom_23_764A9,
        &rom_23_765A9,
        &rom_23_767A9,
        &rom_23_811A9,
        &rom_23_862A9,
        &rom_23_863A9,
        &rom_23_864A9,
        &rom_23_E32A9,
        &rom_23_E33A9,
        &rom_23_E39A9
    };
};


#endif // _M9312_H_