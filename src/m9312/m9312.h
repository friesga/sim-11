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
// The M9312 has sockets for:
// - One Console Emulator and Diagnostics ROM. This ROM contains 256 words
//   and occupies addresses 0765000 upto and including 0765776,
// - Four Bootstrap ROMs. Each ROM contains 64 words, i.e. in total also
//   256 words, occupying addresses 0773000 upto and including 0773776.
//
class M9312 : public PDP11Peripheral
{
public:
    M9312 (Qbus* bus, shared_ptr<M9312Config> m9312Config);

    // Functions required by the BusDevice interface
    void reset () override;
    bool responsible (BusAddress address) override;
    StatusCode read (BusAddress busAddress, u16* data) override;
    StatusCode writeByte (BusAddress busAddress, u8 data) override;
    StatusCode writeWord (BusAddress busAddress, u16 data) override;

private:
    static const u16 diagROMBaseAddress = 0165000;
    static const u16 diagROMSize = 256;
    static const u16 bootROMBaseAddress = 0173000;
    static const u16 bootROMSize = 64;
    static const u16 numberOfBootROMs = 4;
    static const array<u16, numberOfBootROMs> bootROMBaseAddresses;

    // This function converts an address in the range 0173000 - 0173776
    // to a boot ROM number (starting from zero). Addresses in the range
    // 0173000-0173176 are in ROM 0, addresses 173200-173276 in ROM 1, etc.
    // The ROM number therefore is determined by bits 7 and 8 of the address.
    constexpr u16 getBootRomNumber (u16 address) const
    {
        return ((address & 0600) >> 7);
    }

    using DiagROMImage = array<u16, diagROMSize> const;
    using BootROMImage = array<u16, bootROMSize> const;

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

    // A nullptr indicates an empty slot. This can arise if no ROMType is
    // specified in the configuration file. diagnosticROM will then have the
    // value DiagROMType::NONE which translates to zero.
    // 
    array<DiagROMImage*, +M9312Config::DiagROMType::NUMBER> diagROMImages_ =
    {
        nullptr,
        &rom_23_248F1,
        &rom_23_446F1,
        &rom_23_616F1,
        &rom_23_774F1
    };

    array<BootROMImage*, +M9312Config::BootROMType::NUMBER> bootROMImages_ =
    {
        nullptr,
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

    DiagROMImage* diagnosticROM_ {nullptr};
    array<BootROMImage*, numberOfBootROMs> bootROM_ {nullptr};
    u16 addressOffset_ {0};
    bool powerUpViaVector {true};

    bool addressInDiagnosticROM (BusAddress address);
    bool addressInBootRom (BusAddress address);
    bool addressIsPowerfailVector (BusAddress address);
    StatusCode readDiagnosticROM (BusAddress busAddress, u16* data);
    StatusCode readBootROM (BusAddress busAddress, u16* data);
    void BPOKReceiver (bool signalValue);
};

inline const array<u16, M9312::numberOfBootROMs> M9312::bootROMBaseAddresses =
    {0173000, 0173200, 0173400, 0173600};

#endif // _M9312_H_