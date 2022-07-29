#ifndef _RLV12_H_
#define _RLV12_H_

#include "trace.h"
#include "rl01_02/rl012.h"
#include "qbus/qbus.h"
#include "qbusmodule/qbusmodule.h"
#include "asynctimer/asynctimer.h"

#include <array>
#include <memory>       // for std::shared_ptr<>

// From simh pdp11_defs.h
#define IOPAGEBASE      017760000                       // 2**22 - 2**13
#define IOBA_RL         (IOPAGEBASE + 014400)           // RL11
#define VEC_RL          0160

// RLV12 constants
// N.B. RL_NUMCY is the value for a RL01; a RL02 contains 512 cylinders per
// cartridge.
// ToDo: Replace definitions by constants (or enums)
#define RL_NUMWD        (128)                           // Words/sector
#define RL_NUMSC        (40)                            // Sectors/surface
#define RL_NUMSF        (2)                             // Surfaces/cylinder
#define RL_NUMCY        (256)                           // Cylinders/cartridge
#define RL_NUMDRIVES    (4)                             // Drives/controller
#define RL_MAXFR        (RL_NUMSC * RL_NUMWD)           // Max transfer
#define RL01_SIZE (RL_NUMCY * RL_NUMSF * RL_NUMSC * RL_NUMWD) // Words/cartridge
#define RL02_SIZE       (RL01_SIZE * 2)                 // Words/cartridge

// RLCS bits
#define RLCS_DRDY       (0000001)                       // Drive ready 
#define RLCS_M_FUNC     (0000007)                       // Function 
#define  RLCS_NOP       (0)
#define  RLCS_WCHK      (1)
#define  RLCS_GSTA      (2)
#define  RLCS_SEEK      (3)
#define  RLCS_RHDR      (4)
#define  RLCS_WRITE     (5)
#define  RLCS_READ      (6)
#define  RLCS_RNOHDR    (7)
#define  RLCS_SPECIAL   (8)                             // Internal function, drive state 
#define RLCS_V_FUNC     (1)
#define RLCS_M_MEX      (03)                            // Memory extension 
#define RLCS_V_MEX      (4)
#define RLCS_MEX        (RLCS_M_MEX << RLCS_V_MEX)
#define RLCS_M_DRIVE    (03)
#define RLCS_V_DRIVE    (8)
#define RLCS_INCMP      (0002000)                       // Incomplete 
#define RLCS_CRC        (0004000)                       // CRC error 
#define RLCS_HCRC       (RLCS_CRC|RLCS_INCMP)           // Header CRC error 
#define RLCS_DLT        (0010000)                       // Data late 
#define RLCS_HDE        (RLCS_DLT|RLCS_INCMP)           // Header not found error 
#define RLCS_NXM        (0020000)                       // Non-existing memory 
#define RLCS_PAR        (RLCS_NXM|RLCS_INCMP)           // Parity error 
#define RLCS_DRE        (0040000)                       // Drive error 
#define RLCS_ERR        (0100000)                       // Error summary 
#define RLCS_ALLERR     (RLCS_ERR|RLCS_DRE|RLCS_NXM|RLCS_HDE|RLCS_CRC|RLCS_INCMP)
#define RLCS_RW         (0001776)                       // Read/write 
#define GET_FUNC(x)     (((x) >> RLCS_V_FUNC) & RLCS_M_FUNC)
#define GET_DRIVE(x)    (((x) >> RLCS_V_DRIVE) & RLCS_M_DRIVE)

// RLDA 
#define RLDA_GS         (0000002)                       /* get status */
#define RLDA_SK_DIR     (0000004)                       /* direction */
#define RLDA_GS_CLR     (0000010)                       /* clear errors */
#define RLDA_SK_HD      (0000020)                       /* head select */

#define RLDA_V_SECT     (0)                             /* sector */
#define RLDA_M_SECT     (077)
#define RLDA_V_TRACK    (6)                             /* track */
#define RLDA_M_TRACK    (01777)
#define RLDA_HD0        (0 << RLDA_V_TRACK)
#define RLDA_HD1        (1u << RLDA_V_TRACK)
#define RLDA_V_CYL      (7)                             /* cylinder */
#define RLDA_M_CYL      (0777)
#define RLDA_TRACK      (RLDA_M_TRACK << RLDA_V_TRACK)
#define RLDA_CYL        (RLDA_M_CYL << RLDA_V_CYL)

// ToDo: Convert macro's to inline functions
#define GET_SECT(x)     (((x) >> RLDA_V_SECT) & RLDA_M_SECT)
#define GET_CYL(x)      (((x) >> RLDA_V_CYL) & RLDA_M_CYL)
#define GET_TRACK(x)    (((x) >> RLDA_V_TRACK) & RLDA_M_TRACK)
#define GET_DA(x)       ((GET_TRACK (x) * RL_NUMSC) + GET_SECT (x))

// RLBA
#define RLBA_IMP        (0177777)                       // Implemented bits

// RLBAE 
#define RLBAE_IMP       (0000077)                       // Implemented bits

// Unibus and cpu_opt are currently unsupported
#define UNIBUS          0

// Device flags
#define DEV_V_UF        16                              // User flags
#define DEV_V_RLV11     (DEV_V_UF + 7)                  // RLV11
#define DEV_RLV11       (1u << DEV_V_RLV11)

// Implementation of the RL11, RLV11 and RLV12 controllers.
class RLV12 : public QBUSModule
{
    // Define RLV12 registers as offsets from the controllers bas address
    enum
    { 
        CSR = 00,
        BAR = 02,
        DAR = 04,
        MPR = 06,
        BAE = 010
    };

    // Define controller registers
    u16 rlcs;       // Control/status register
    u16 rlba;       // Bus Address register
    u16 rlda;       // Disk address register
    u16 rlmpr;      // Multi purpose regsister
    u16 rlbae;      // Bus Address Extension register

    u16 rlmpr1 = 0; // MPR register queue
    u16 rlmpr2 = 0;

    // Timing constants
    int32_t const rl_swait = 10;    // Seek wait

    // Define transfer buffer
    u16 *rlxb_;

    // Define device flags
    // ToDo: Refactor bit flags
    uint32_t flags_;
    
    // A RLV12 can have a maximum of four units
    std::array<RL01_2, RL_NUMDRIVES> units_
    {
        RL01_2 (this),
        RL01_2 (this),
        RL01_2 (this),
        RL01_2 (this)
    };

    // ToDo: Make this a global timer to be used by all devices?!
    AsyncTimer<RL01_2> timer{};

    // Private function
    void maintenance ();
    u16 calcCRC (int const wc, u16 const *data);
    void setDone (int32_t status);
    inline int32_t getCylinder (int32_t track);

public:
    // Constructor/destructor
    RLV12 ();
    ~RLV12 ();

    // Required functions
    void reset () override;
    StatusCode read (u16 registerAddress, u16* data) override;
    StatusCode writeByte (u16 registerAddress, u8 data) override;
    StatusCode writeWord (u16 registerAddress, u16 data) override;
    void service (RL01_2 &unit);
};

#endif _RLV12_H_