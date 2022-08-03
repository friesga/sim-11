#ifndef _RL012_H_
#define _RL012_H_

#include "unit/unit.h"
#include "types.h"
#include "statuscodes.h"

// Flags in the unit flags word
// ToDo: Replace #define by static constexpr 
// ToDo: The RLV12 device should have its own set of flags
#define UNIT_V_UF       16                              // Device specific  unit flags 

#define UNIT_V_WLK      (UNIT_V_UF + 0)                 // hwre write lock 
#define UNIT_V_RL02     (UNIT_V_UF + 1)                 // RL01 vs RL02 
#define UNIT_V_AUTO     (UNIT_V_UF + 2)                 // autosize enable 
#define UNIT_V_DUMMY    (UNIT_V_UF + 3)                 // dummy flag, for SET BADBLOCK 
#define UNIT_V_OFFL     (UNIT_V_UF + 4)                 // unit off line 
#define UNIT_V_BRUSH    (UNIT_V_UF + 5)                 // unit has brushes 

#define UNIT_WLK        (1u << UNIT_V_WLK)
#define UNIT_RL02       (1u << UNIT_V_RL02)
#define UNIT_AUTO       (1u << UNIT_V_AUTO)
#define UNIT_BRUSH      (1u << UNIT_V_BRUSH)
#define UNIT_OFFL       (1u << UNIT_V_OFFL)
#define UNIT_DUMMY      (1u << UNIT_V_DUMMY)

#define UNIT_WPRT       (UNIT_WLK | UNIT_RO)            // write protected 

// RLDS 
// NI = not implemented,
// * = kept in status_,
// ^ = kept in currentTrack_,
// ! = kept in uptr

#define RLDS_M_STATE    (07)
#define RLDS_LOAD       (0)                             // no cartridge 
#define RLDS_SPIN       (1)                             // spin-up 
#define RLDS_BRUSH      (2)                             // brush cycle *! 
#define RLDS_HLOAD      (3)                             // load heads 
#define RLDS_SEEK       (4)                             // drive seeking * 
#define RLDS_LOCK       (5)                             // lock on * 
#define RLDS_UNL        (6)                             // unload heads 
#define RLDS_DOWN       (7)                             // spin-down 
#define RLDS_BHO        (0000010)                       // brushes home * 
#define RLDS_HDO        (0000020)                       // heads out * 
#define RLDS_CVO        (0000040)                       // cover open * 
#define RLDS_HD         (0000100)                       // head select ^ 
#define RLDS_RL02       (0000200)                       // RL02 ! 
#define RLDS_DSE        (0000400)                       // drv sel err 
#define RLDS_VCK        (0001000)                       // vol check * 
#define RLDS_WGE        (0002000)                       // wr gate err * 
#define RLDS_SPE        (0004000)                       // spin err * 
#define RLDS_STO        (0010000)                       // seek time out * 
#define RLDS_WLK        (0020000)                       // wr locked ! 
#define RLDS_HCE        (0040000)                       // hd curr err NI 
#define RLDS_WDE        (0100000)                       // wr data err NI 
#define RLDS_ERR        (RLDS_WDE|RLDS_HCE|RLDS_STO|RLDS_SPE|RLDS_WGE| \
                         RLDS_VCK|RLDS_DSE)             // errors bits 

// RLO1/RL02 unit
class RL01_2 : public Unit
{
    friend class RLV12;

    // ToDo: Split currentTrackHeadSector_ in three separate variables
    int32_t currentTrackHeadSector_;
    int32_t status_;
    int32_t function_;

public:
    // Constructor
    RL01_2 (BusDevice *owningDevice);

    // Required functions
    StatusCode attach (std::string fileName) override;

    // Make the class a function object
    void operator() ();
};


#endif // _RL012_H_