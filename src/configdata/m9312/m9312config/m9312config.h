#ifndef _M9312CONFIG_H_
#define _M9312CONFIG_H_

#include "devicetype.h"

#include <array>

using std::array;

// Five sockets on the M9312 allow the user to interchange ROMs, enabling
// the module to be used with any Unibus PDP-11 system and boot any peripheral
// device by simply adding or changing ROMs. One socket is solely used for a
// diagnostic ROM (PDP-11/60 and 11/70 systems) or a ROM which contains the
// console emulator routine and diagnostics for all other PDP-11 systems.The
// other four sockets accept ROMs which contain bootstrap programs.One or two
// bootstrap programs may be contained in a particular ROM; however, some
// devices may require two or more ROMs to contain their particular bootstrap
// programs.
//
struct M9312Config : public DeviceType<BusType::Unibus>
{
    enum class ROMType
    {
        NONE,        // Default socket contents
        _23_000F1,   // 11/00 Diagnostic/Console (UBI; M7098 E58)
        _23_248F1,   // (A0) 11/04,34 Diagnostic/Console (M9312 E20)
        _23_446F1,   // 11/44 Diagnostic/Console (UBI; M7098 E58)
        _23_616F1,   // (B0) 11/60,70 Diagnostic (M9312 E20)
        _23_774F1,   // 11/24 Diagnostic/Console (MEM; M7134 E74)
        _23_751A9,   // (DL) RL01/02 cartridge disk boot ROM
        _23_752A9,   // (DM) RK06/07 cartridge disk
        _23_753A9,   // (DX) RX01 floppy disk, single density
        _23_755A9,   // (DP) RP02/03 cartridge disk
                     // (DB) RP04/05/06,RM02/03/05 cartridge disk
        _23_756A9,   // (DK) RK03/05 DECdisk
                     // (DT) TU55/56 DECtape
        _23_757A9,   // (MM) TU16/45/77,TE16 magtape
        _23_758A9,   // (MT) TS03,TU10,TE10 magtape
        _23_759A9,   // (DS) RS03/04 fixed disk
        _23_760A9,   // (TT) ASR33 lowspeed reader
                     // (PR) PC05 hispeed reader
        _23_761A9,   // (CT) TU60 DECcassette
        _23_764A9,   // (MS) TS04/11,TU80,TSU05 tape
        _23_765B9,   // (DD) TU58 DECtape II; fixes non-std CSR access
        _23_767A9,   // (DU) MSCP UDA50(RAxx) disk
        _23_811A9,   // (DY) RX02 floppy disk, double density
        _23_862A9,   // (XM) DECnet DDCMP DMC11 / DMR11 - ROM0
        _23_863A9,   // (XM) DECnet DDCMP DMC11 / DMR11 - ROM1
        _23_864A9,   // (XM) DECnet DDCMP DMC11 / DMR11 - ROM2
        _23_E32A9,   // (XE) Ethernet DEUNA/DELUA Net Boot (v2) ROM0
        _23_E33A9,   // (XE) Ethernet DEUNA/DELUA Net Boot (v2) ROM1
        _23_E39A9,   // (MU) TMSCP TK50,TU81 magtape
    };

    ROMType diagnosticROM {ROMType::NONE};      // Console emulator and diagnostic ROM
    array<ROMType, 4> bootROM {ROMType::NONE};  // Bootstrap ROM's
    u16 startingAddress {0};                    // Bootstrap address
};

#endif // _M9312CONFIG_H_