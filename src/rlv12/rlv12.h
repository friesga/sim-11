#ifndef _RLV12_H_
#define _RLV12_H_

#include "trace/trace.h"
#include "rl01_02/rl012.h"
#include "qbus/qbus.h"
#include "busdevice/busdevice.h"
#include "threadsafecontainers/threadsafequeue.h"
#include "statuscodes.h"
#include "rlv12command/rlv12command.h"
#include "configdata/include/configdata.h"
#include "cmdprocessor/cmdprocessor.h"

#include <array>
#include <memory>               // for std::unique_ptr<>
#include <thread>               // for std::thread
#include <mutex>                // for std::mutex
#include <condition_variable>   // for std::condition_variable

// From simh pdp11_defs.h
// IOPAGEBASE for a 22-bit system
// #define IOPAGEBASE      017760000                    // 2**22 - 2**13
// IOPAGEBASE for a 16-bit system
// #define IOPAGEBASE      0160000 

// #define IOBA_RL         (IOPAGEBASE + 014400)           // RL11
// #define VEC_RL          0160

// RLV12 constants
// N.B. RL_NUMCY is the value for a RL01; a RL02 contains 512 cylinders per
// cartridge.
// ToDo: Replace definitions by constants (or enums)
// #define RL_NUMWD        (128)                           // Words/sector
// #define RL_NUMSC        (40)                            // Sectors/surface
// #define RL_NUMSF        (2)                             // Surfaces/cylinder
// #define RL_NUMCY        (256)                           // Cylinders/cartridge
// #define RL_NUMDRIVES    (4)                             // Drives/controller
// #define RL_MAXFR        (RL_NUMSC * RL_NUMWD)           // Max transfer
// #define RL01_SIZE (RL_NUMCY * RL_NUMSF * RL_NUMSC * RL_NUMWD) // Words/cartridge
// #define RL02_SIZE       (RL01_SIZE * 2)                 // Words/cartridge

// RLCS bits
// ToDo: Change RLCS_ macro's to enum
// #define RLCS_DRDY           (0000001)                   // Drive ready 
// #define RLCS_M_FUNC         (0000007)                   // Function 
// #define RLCS_MAINTENANCE    (0)
// #define RLCS_WCHK           (1)
// #define RLCS_GSTA           (2)
// #define RLCS_SEEK           (3)
// #define RLCS_RHDR           (4)
// #define RLCS_WRITE          (5)
// #define RLCS_READ           (6)
// #define RLCS_RNOHDR         (7)
// #define RLCS_SPECIAL        (8)                        // Internal function, drive state 

// #define RLCS_V_FUNC     (1)
// #define RLCS_M_MEX      (03)                            // Memory extension 
// #define RLCS_V_MEX      (4)
// #define RLCS_MEX        (RLCS_M_MEX << RLCS_V_MEX)
// #define RLCS_M_DRIVE    (03)
// #define RLCS_V_DRIVE    (8)
// #define RLCS_INCMP      (0002000)                       // Incomplete 
// #define RLCS_CRC        (0004000)                       // CRC error 
// #define RLCS_HCRC       (RLCS_CRC|RLCS_INCMP)           // Header CRC error 
// #define RLCS_DLT        (0010000)                       // Data late 
// #define RLCS_HNF        (RLCS_DLT|RLCS_INCMP)           // Header not found error 
// #define RLCS_NXM        (0020000)                       // Non-existing memory 
// #define RLCS_PAR        (RLCS_NXM|RLCS_INCMP)           // Parity error 
// #define RLCS_DRE        (0040000)                       // Drive error 
// #define RLCS_ERR        (0100000)                       // Error summary 
// #define RLCS_ALLERR     (RLCS_DRE|RLCS_NXM|RLCS_HNF|RLCS_CRC|RLCS_INCMP)
// #define RLCS_RW         (0001776)                       // Read/write 
// #define GET_FUNC(x)     (((x) >> RLCS_V_FUNC) & RLCS_M_FUNC)
// #define GET_DRIVE(x)    (((x) >> RLCS_V_DRIVE) & RLCS_M_DRIVE)

// RLDA 
// #define RLDA_GS         (0000002)                       /* get status */
// #define RLDA_SK_DIR     (0000004)                       /* direction */
// #define RLDA_GS_CLR     (0000010)                       /* clear errors */
// #define RLDA_SK_HD      (0000020)                       /* head select */

// #define RLDA_V_SECT     (0)                             /* sector */
// #define RLDA_M_SECT     (077)
// #define RLDA_V_TRACK    (6)                             /* track */
// #define RLDA_M_TRACK    (01777)
// #define RLDA_HD0        (0 << RLDA_V_TRACK)
// #define RLDA_HD1        (1u << RLDA_V_TRACK)
// #define RLDA_V_CYL      (7)                             /* cylinder */
// #define RLDA_M_CYL      (0777)
// #define RLDA_TRACK      (RLDA_M_TRACK << RLDA_V_TRACK)
// #define RLDA_CYL        (RLDA_M_CYL << RLDA_V_CYL)

// RLBA
// #define RLBA_IMP        (0177777)                       // Implemented bits

// RLBAE 
// #define RLBAE_IMP       (0000077)                       // Implemented bits

// Forward declaration
class CmdProcessor;

// Implementation of the RL11, RLV11 and RLV12 controllers.
class RLV12 : public BusDevice
{
    friend class CmdProcessor;
    friend class RL01_2;

    // RL01 Recording Capacity definitions
    // An RL02 drive has 512 cylinders per cartridge
    static constexpr int32_t wordsPerSector        = 128; // RL_NUMWD
    static constexpr int32_t sectorsPerSurface     = 40;  // RL_NUMSC
    static constexpr int32_t surfacesPerCylinder   = 2;   // RL_NUMSF
    static constexpr int32_t cylindersPerCartridge = 256; // RL_NUMCY

    static constexpr int32_t maxDrivesPerController = 4;  // RL_NUMDRIVES
    static constexpr int32_t maxTransfer = sectorsPerSurface * wordsPerSector; // RL_MAXFR

    static constexpr int32_t RL01_WordsPerCartridge = 
        cylindersPerCartridge * surfacesPerCylinder * sectorsPerSurface *
        wordsPerSector;                                 // RL01_SIZE
    static constexpr int32_t RL02_WordsPerCartridge = 
        RL01_WordsPerCartridge * 2;                     // RL02_SIZE

    // Define the address of the IOPage for a 16-bit system. The definition
    // for a 22-bit system woule be 017760000 (i.e. 2**22 - 2**13).
    static constexpr u32 IOPageBase = 0160000;          // IOPAGEBASE

    // Define the default base address and vector of the device
    static constexpr u32 RLV_BaseAddress = IOPageBase + 014400; // IOBA_RL
    static constexpr u32 RLV_Vector = 0160;                     // VEC_RL
    
    // Define RLV12 registers as offsets from the controllers base address
    enum
    { 
        CSR = 00,
        BAR = 02,
        DAR = 04,
        MPR = 06,
        BAE = 010
    };

    // CSR bit definitions
    // The function definitions are unused as the functions to execute
    // are indexed via vector in the command processor (except for CSR_Seek
    // which needs special treatment).
    static constexpr u16 CSR_DriveReady   = 1;  
    static constexpr u16 CSR_Maintenance  = 0;  
    static constexpr u16 CSR_WriteCheck   = 1;  
    static constexpr u16 CSR_GetStatus    = 2;  
    static constexpr u16 CSR_Seek         = 3;  
    static constexpr u16 CSR_ReadHeader   = 4;  
    static constexpr u16 CSR_WriteData    = 5;  
    static constexpr u16 CSR_ReadData     = 6;  
    static constexpr u16 CSR_ReadNoHeader = 7;  
    static constexpr u16 CSR_Special      = 8;  

    static constexpr u16 CSR_AddressExtPosition = 4;
    static constexpr u16 CSR_AddressExtBits = 3;
    static constexpr u16 CSR_AddressExtField = 
        CSR_AddressExtBits << CSR_AddressExtPosition;

    static constexpr u16 getFunction (u16 csr)
    {
        constexpr u16 CSR_FunctionPosition = 1;
        constexpr u16 CSR_FunctionBits     = 7;
        return (csr >> CSR_FunctionPosition) & CSR_FunctionBits;
    }

    static constexpr u16 getDrive (u16 csr)
    {
        constexpr u16 CSR_DrivePosition = 8;
        constexpr u16 CSR_DriveBits  = 3;
        return (csr >> CSR_DrivePosition) & CSR_DriveBits;
    }

    static constexpr u16 CSR_InterruptEnable = 1u << 6;     // CSR_IE
    static constexpr u16 CSR_ControllerReady = 1u << 7;     // CSR_DONE
    // CSR Error bit definitions. Not all error codes are actually used.
    static constexpr u16 CSR_ErrorCodePosition   = 10;
    static constexpr u16 CSR_OperationIncomplete = 1 << CSR_ErrorCodePosition; // RLCS_INCMP
    static constexpr u16 CSR_ReadDataCRC         = 2 << CSR_ErrorCodePosition; // RLCS_CRC
    static constexpr u16 CSR_HeaderCRC           = 3 << CSR_ErrorCodePosition; // RLCS_HCRC
    static constexpr u16 CSR_DataLate            = 4 << CSR_ErrorCodePosition; // RLCS_DLT
    static constexpr u16 CSR_HeaderNotFound      = 5 << CSR_ErrorCodePosition; // RLCS_HNF
    static constexpr u16 CSR_NonExistentMemory   = 8 << CSR_ErrorCodePosition; // RLCS_NXM
    static constexpr u16 CSR_MemoryParityError   = 9 << CSR_ErrorCodePosition; // RLCS_PAR
    static constexpr u16 CSR_DriveError          = 1u << 14;                   // RLCS_DRE
    static constexpr u16 CSR_CompositeError      = 1u << 15;                   // RLCS_ERR
    static constexpr u16 CSR_AnyError            = 
        CSR_DriveError  | CSR_NonExistentMemory | CSR_HeaderNotFound |
        CSR_ReadDataCRC | CSR_OperationIncomplete;                             // RLCS_ALLERR
    static constexpr u16 CSR_ReadWriteBits       = 0001776;                    // RLCS_RW

    // Disk Address Register bit definitions
    // Note that the registers contents can have different meanings depending
    // on the function being performed.
    static constexpr u16 DAR_GetStatus  = 0000002;   // RLDA_GS
    static constexpr u16 DAR_Direction  = 0000004;   // RLDA_SK_DIR
    static constexpr u16 DAR_Reset      = 0000010;   // RLDA_GS_CLR
    static constexpr u16 DAR_HeadSelect = 0000020;   // RLDA_SK_HD

    static constexpr u16 DAR_SectorPosition   = 0;            // RLDA_V_SECT
    static constexpr u16 DAR_SectorBits       = 077;          // RLDA_M_SECT
    static constexpr u16 DAR_Sector = 
        DAR_SectorBits << DAR_SectorPosition;
    
    static constexpr u16 DAR_TrackPosition    = 6;            // RLDA_V_TRACK
    static constexpr u16 DAR_TrackBits        = 01777;        // RLDA_M_TRACK
    static constexpr u16 DAR_Track = 
        DAR_TrackBits << DAR_TrackPosition;                   // RLDA_TRACK
    
    static constexpr u16 DAR_Head0 = 0 << DAR_TrackPosition;  // RLDA_HD0
    static constexpr u16 DAR_Head1 = 1u << DAR_TrackPosition; // RLDA_HD1

    static constexpr u16 DAR_CylinderPosition = 7;            // RLDA_V_CYL
    static constexpr u16 DAR_CylinderBits     = 0777;         // RLDA_M_CYL
    static constexpr u16 DAR_Cylinder = 
        DAR_CylinderBits << DAR_CylinderPosition;             // RLDA_CYL


    static constexpr int32_t getCylinder (int32_t diskAddress)
    { return (diskAddress >> DAR_CylinderPosition) & DAR_CylinderBits; }

    static constexpr int32_t getTrack (int32_t diskAddress)
    { return (diskAddress >> DAR_TrackPosition) & DAR_TrackBits; }

    static constexpr int32_t getSector (int32_t diskAddress) 
    { return (diskAddress >> DAR_SectorPosition) & DAR_SectorBits; }

    static constexpr int32_t getBlockNumber (int32_t diskAddress)
    { 
        return ((getTrack (diskAddress) * sectorsPerSurface) + 
            getSector (diskAddress));
    }

    // Multipurpose Register
    //
    // Bit layout for the MPR during a Get Status Command. These definitions
    // are also used in the units drive status.
    // 
    // Bits 0:2 define the state of the drive (STA, STB and STC).
    static constexpr u16 MPR_GS_State         = 07;     // RLDS_M_STATE
    static constexpr u16 MPR_GS_LoadCartridge = 0;      // RLDS_LOAD
    static constexpr u16 MPR_GS_SpinUp        = 1;      // RLDS_SPIN
    static constexpr u16 MPR_GS_BrushCycle    = 2;      // RLDS_BRUSH
    static constexpr u16 MPR_GS_LoadHeads     = 3;      // RLDS_HLOAD
    static constexpr u16 MPR_GS_Seek          = 4;      // RLDS_SEEK
    static constexpr u16 MPR_GS_LockOn        = 5;      // RLDS_LOCK
    static constexpr u16 MPR_GS_UnloadHeads   = 6;      // RLDS_UNL
    static constexpr u16 MPR_GS_SpinDown      = 7;      // RLDS_DOWN
    // Definition of MPR bits 3:15
    static constexpr u16 MPR_GS_BrushHome        = 0000010; // RLDS_BHO
    static constexpr u16 MPR_GS_HeadsOut         = 0000020; // RLDS_HDO
    static constexpr u16 MPR_GS_CoverOpen        = 0000040; // RLDS_CVO
    static constexpr u16 MPR_GS_HeadSelect       = 0000100; // RLDS_HD
    static constexpr u16 MPR_GS_DriveType        = 0000200; // RLDS_RL02
    static constexpr u16 MPR_GS_DriveSelectError = 0000400; // RLDS_DSE
    static constexpr u16 MPR_GS_VolumeCheck      = 0001000; // RLDS_VCK
    static constexpr u16 MPR_GS_WriteGateError   = 0002000; // RLDS_WGE
    static constexpr u16 MPR_GS_SpinError        = 0004000; // RLDS_SPE
    static constexpr u16 MPR_GS_SeekTimeOut      = 0010000; // RLDS_STO
    static constexpr u16 MPR_GS_WriteLock        = 0020000; // RLDS_WLK
    static constexpr u16 MPR_GS_CurrentHeadError = 0040000; // RLDS_HCE
    static constexpr u16 MPR_GS_WriteDataError   = 0100000; // RLDS_WDE
    static constexpr u16 MPR_GS_AnyError = 
        MPR_GS_WriteDataError | MPR_GS_CurrentHeadError | 
        MPR_GS_WriteLock      | MPR_GS_SeekTimeOut      |
        MPR_GS_SpinError      | MPR_GS_WriteGateError   | 
        MPR_GS_VolumeCheck    | MPR_GS_DriveSelectError;    // RLDS_ERR

    // DR RDY will be negated [i.e. cleared] when a drive error
    // occurs except when an attempt has been made to write on a
    // write-protected drive or if volume check is set.
    // (EK-RLV12-TD-001, p3-7)
    static constexpr u16 DriveReadyNegate = 
        MPR_GS_WriteDataError |MPR_GS_CurrentHeadError |MPR_GS_SeekTimeOut | 
        MPR_GS_SpinError      |MPR_GS_WriteGateError   |MPR_GS_DriveSelectError;

    // Bus Address Register
    static constexpr u16 BAR_Bits = 0177777;           // RLBA_IMP

    // Bus Address Extension register
    static constexpr u16 BAE_Bits = 0000077;           // RLBAE_IMP

    // Define controller registers
    // The MPR is not a single physical entity like the other registers. It
    // consists of two separate registers, the word counter and the FIFO output
    // buffer, both bearing the same base address. (EK-RLV12-TD-001 pg. 3-4)
    u16 rlcs;       // Control/status register
    u16 rlba;       // Bus Address register
    u16 rlda;       // Disk address register
    u16 rlbae;      // Bus Address Extension register


    // Timing constants
    // ToDo: rl_swait is superfluous
    int32_t const rl_swait = 10;    // Seek wait

    // Define transfer buffer
    u16 *rlxb_;

    // Definition of the controller type and the presence of the 22-bit option
    // when configured as a RLV12 controller.
    RlConfig::RLType rlType_;
    bool _22bit_;
    
    // A RLV12 can have a maximum of four units. RL01_2 objects for
    // which no unit is connected to the controller will have the status
    // off-line.
    std::array<RL01_2, maxDrivesPerController> units_
    {
        RL01_2 (this),
        RL01_2 (this),
        RL01_2 (this),
        RL01_2 (this)
    };

    // Definition of the pointer to the command processor
    std::unique_ptr<CmdProcessor> cmdProcessor;

    // Safe guard against controller access from multiple service threads
    std::mutex controllerMutex_;

    // Condition variable to wake up the command processor
    std::condition_variable signal;

    // The MPR consists of two separate registers, the word counter and the
    // FIFO output buffer, both bearing the same base address. The word
    // counter is a write-only register, while the output buffer is a read-only
    // register.
    // When writing the MPR, the data word is loaded into the word counter (WC)
    // register. [...] When reading the MPR, the data word is read from the
    // FIFO output buffer. (EK-RLV-12-TD-001, pg. 4-3)

    // Word counter
    u16 wordCounter_;

    // Index into the FIFO buffer for reading the FIFO via the MPR
    size_t fifoIndex_;

    // Private functions
    u16 calcCRC (int const wc, u16 const *data);
    void setDone (RL01_2 &unit, u16 status);

public:
    // Constructors/destructor
    RLV12 ();
    RLV12 (RlConfig *rlConfig);
    /*
        RLV12 (u32 baseAddress, u32 vector, bool rlv11, bool _22bit, 
        size_t numUnits);
    */
    ~RLV12 ();

    // Required functions
    void reset () override;
    bool responsible (u16 addr) override;
    StatusCode read (u16 registerAddress, u16* data) override;
    StatusCode writeByte (u16 registerAddress, u8 data) override;
    StatusCode writeWord (u16 registerAddress, u16 data) override;

    // RLV1[12] specific function
    inline size_t numUnits ();
    inline RL01_2  *unit (size_t unitNumber);
    void service (Unit &unit, 
        ThreadSafeQueue<std::shared_ptr<RLV12Command>> &commandQueue);

    // Functions to set and get memory adresses consistently for
    // 16-, 18- and 22-bit systems
    void memAddrToRegs (u32 memoryAddress);
    u32 memAddrFromRegs ();
    void updateBAE ();
    u16 rlcsPlusDriveStatus (RL01_2 &unit);
    constexpr u16 getBA16BA17 (u16 csr);
};


inline size_t RLV12::numUnits ()
{
    return maxDrivesPerController;
}
 
inline RL01_2  *RLV12::unit (size_t unitNumber)
{
    return &units_[unitNumber];
}


#endif // _RLV12_H_