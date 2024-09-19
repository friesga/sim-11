#ifndef _RLV12CONST_H_
#define _RLV12CONST_H_

#include "types.h"

// Definition of register lay-out and drive geometry for the RLV12.
namespace RLV12const
{
    // RL01 Recording Capacity definitions
    // An RL02 drive has 512 cylinders per cartridge
    static constexpr int32_t wordsPerSector = 128;
    static constexpr int32_t sectorsPerSurface = 40;
    static constexpr int32_t surfacesPerCylinder = 2;
    static constexpr int32_t cylindersPerCartridge = 256;

    static constexpr int32_t maxDrivesPerController = 4;
    static constexpr int32_t maxTransfer = sectorsPerSurface * wordsPerSector;

    static constexpr int32_t RL01_WordsPerCartridge =
        cylindersPerCartridge * surfacesPerCylinder * sectorsPerSurface *
        wordsPerSector;
    static constexpr int32_t RL02_WordsPerCartridge =
        RL01_WordsPerCartridge * 2;

    // Define the base address of the registers in the I/O page as
    // a 16-bit address.
    static constexpr u16 IOPageBase = 0160000;

    // Define the default base address and vector of the device
    static constexpr u16 RLV_BaseAddress = IOPageBase + 014400;
    static constexpr u16 RLV_Vector = 0160;

    // CSR bit definitions
    // The function definitions are unused as the functions to execute
    // are indexed via vector in the command processor (except for CSR_Seek
    // which needs special treatment).
    static constexpr u16 CSR_DriveReady = 1;
    static constexpr u16 CSR_Maintenance = 0;
    static constexpr u16 CSR_WriteCheck = 1;
    static constexpr u16 CSR_GetStatus = 2;
    static constexpr u16 CSR_Seek = 3;
    static constexpr u16 CSR_ReadHeader = 4;
    static constexpr u16 CSR_WriteData = 5;
    static constexpr u16 CSR_ReadData = 6;
    static constexpr u16 CSR_ReadNoHeader = 7;
    static constexpr u16 CSR_Special = 8;

    static constexpr u16 CSR_AddressExtPosition = 4;
    static constexpr u16 CSR_AddressExtMask = 3;
    static constexpr u16 CSR_AddressExtension =
        CSR_AddressExtMask << CSR_AddressExtPosition;

    static constexpr u8 getFunction (u16 csr_)
    {
        constexpr u16 CSR_FunctionPosition = 1;
        constexpr u16 CSR_FunctionMask = 7;
        return (csr_ >> CSR_FunctionPosition) & CSR_FunctionMask;
    }

    static constexpr u16 getDrive (u16 csr_)
    {
        constexpr u16 CSR_DrivePosition = 8;
        constexpr u16 CSR_DriveMask = 3;
        return (csr_ >> CSR_DrivePosition) & CSR_DriveMask;
    }

    static constexpr u16 CSR_InterruptEnable = 1u << 6;
    static constexpr u16 CSR_ControllerReady = 1u << 7;
    // CSR Error bit definitions. Not all error codes are actually used.
    static constexpr u16 CSR_ErrorCodePosition = 10;
    static constexpr u16 CSR_OperationIncomplete = 1 << CSR_ErrorCodePosition;
    static constexpr u16 CSR_ReadDataCRC = 2 << CSR_ErrorCodePosition;
    static constexpr u16 CSR_HeaderCRC = 3 << CSR_ErrorCodePosition;
    static constexpr u16 CSR_DataLate = 4 << CSR_ErrorCodePosition;
    static constexpr u16 CSR_HeaderNotFound = 5 << CSR_ErrorCodePosition;
    static constexpr u16 CSR_NonExistentMemory = 8 << CSR_ErrorCodePosition;
    static constexpr u16 CSR_MemoryParityError = 9 << CSR_ErrorCodePosition;
    static constexpr u16 CSR_DriveError = 1u << 14;
    static constexpr u16 CSR_CompositeError = 1u << 15;
    static constexpr u16 CSR_AnyError =
        CSR_DriveError | CSR_NonExistentMemory | CSR_HeaderNotFound |
        CSR_ReadDataCRC | CSR_OperationIncomplete;
    static constexpr u16 CSR_ReadWriteBits = 0001776;

    // Disk Address Register bit definitions
    // Note that the registers contents can have different meanings depending
    // on the function being performed.
    static constexpr u16 DAR_GetStatus = 0000002;
    static constexpr u16 DAR_Direction = 0000004;
    static constexpr u16 DAR_Reset = 0000010;
    static constexpr u16 DAR_HeadSelect = 0000020;

    static constexpr u16 DAR_SectorPosition = 0;
    static constexpr u16 DAR_SectorMask = 077;
    static constexpr u16 DAR_Sector =
        DAR_SectorMask << DAR_SectorPosition;

    static constexpr u16 DAR_TrackPosition = 6;
    static constexpr u16 DAR_TrackMask = 01777;
    static constexpr u16 DAR_Track =
        DAR_TrackMask << DAR_TrackPosition;

    static constexpr u16 DAR_Head0 = 0 << DAR_TrackPosition;
    static constexpr u16 DAR_Head1 = 1u << DAR_TrackPosition;

    static constexpr u16 DAR_CylinderPosition = 7;
    static constexpr u16 DAR_CylinderMask = 0777;
    static constexpr u16 DAR_Cylinder =
        DAR_CylinderMask << DAR_CylinderPosition;


    static constexpr int32_t getCylinder (int32_t diskAddress)
    {
        return (diskAddress >> DAR_CylinderPosition) & DAR_CylinderMask;
    }

    static constexpr int32_t getTrack (int32_t diskAddress)
    {
        return (diskAddress >> DAR_TrackPosition) & DAR_TrackMask;
    }

    static constexpr int32_t getSector (int32_t diskAddress)
    {
        return (diskAddress >> DAR_SectorPosition) & DAR_SectorMask;
    }

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
    static constexpr u16 MPR_GS_State = 07;
    static constexpr u16 MPR_GS_LoadCartridge = 0;
    static constexpr u16 MPR_GS_SpinUp = 1;
    static constexpr u16 MPR_GS_BrushCycle = 2;
    static constexpr u16 MPR_GS_LoadHeads = 3;
    static constexpr u16 MPR_GS_Seek = 4;
    static constexpr u16 MPR_GS_LockOn = 5;
    static constexpr u16 MPR_GS_UnloadHeads = 6;
    static constexpr u16 MPR_GS_SpinDown = 7;
    // Definition of MPR bits 3:15
    static constexpr u16 MPR_GS_BrushHome = 0000010;
    static constexpr u16 MPR_GS_HeadsOut = 0000020;
    static constexpr u16 MPR_GS_CoverOpen = 0000040;
    static constexpr u16 MPR_GS_HeadSelect = 0000100;
    static constexpr u16 MPR_GS_DriveType = 0000200;
    static constexpr u16 MPR_GS_DriveSelectError = 0000400;
    static constexpr u16 MPR_GS_VolumeCheck = 0001000;
    static constexpr u16 MPR_GS_WriteGateError = 0002000;
    static constexpr u16 MPR_GS_SpinError = 0004000;
    static constexpr u16 MPR_GS_SeekTimeOut = 0010000;
    static constexpr u16 MPR_GS_WriteLock = 0020000;
    static constexpr u16 MPR_GS_CurrentHeadError = 0040000;
    static constexpr u16 MPR_GS_WriteDataError = 0100000;
    static constexpr u16 MPR_GS_AnyError =
        MPR_GS_WriteDataError | MPR_GS_CurrentHeadError |
        MPR_GS_WriteLock | MPR_GS_SeekTimeOut |
        MPR_GS_SpinError | MPR_GS_WriteGateError |
        MPR_GS_VolumeCheck | MPR_GS_DriveSelectError;

    // DR RDY will be negated [i.e. cleared] when a drive error
    // occurs except when an attempt has been made to write on a
    // write-protected drive or if volume check is set.
    // (EK-RLV12-TD-001, p3-7)
    static constexpr u16 DriveReadyNegate =
        MPR_GS_WriteDataError | MPR_GS_CurrentHeadError | MPR_GS_SeekTimeOut |
        MPR_GS_SpinError | MPR_GS_WriteGateError | MPR_GS_DriveSelectError;

    // Bus Address Register
    static constexpr u16 BAR_Mask = 0177777;

    // Bus Address Extension register
    static constexpr u16 BAE_Mask = 0000077;
};

#endif // _RLV12CONST_H_