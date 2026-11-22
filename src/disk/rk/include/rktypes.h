#ifndef _RKTYPES_H_
#define _RKTYPES_H_

#include "bitfield.h"
#include "basicregister/basicregister.h"
#include "basicregister/registerfield.h"
#include "busaddress/busaddress.h"
#include "concepts/geometry/geometry.h"
#include "statuscodes.h"

#include <variant>

using std::variant;

namespace RKTypes
{
    // Definition of the RK05 drive format:
    // - 12 sectors/track
    // - 2 disk surfaces/disk
    // - 203 cylinders/disk drive
    // - 256 words/sector
    u16 const SectorsPerSurface = 12;
    u16 const NumberOfHeads = 2;
    u16 const CylindersPerDisk = 203;
    u16 const WordsPerSector = 256;

    // Definition of the RK05 drive format
    Geometry const rk05Geometry_ {RKTypes::SectorsPerSurface,
        RKTypes::NumberOfHeads, RKTypes::CylindersPerDisk,
        RKTypes::WordsPerSector};

    constexpr size_t wordCountForEntireSectors (u16 wordCount)
    { return (wordCount + (WordsPerSector - 1)) & ~(WordsPerSector - 1);}

    // Definition of the RK11-D register bit assignments
    // 
    // Drive Status register. This is a read-only register and contains the
    // status of the selected drive and current sector address.
    //
    class RKDS : public BasicRegister
    {
    public:
        using SectorCounter = BitField<u16, 0, 4>;
        using SectorCounterEqualsSectorAddress = BitField<u16, 4>;
        using WriteProtectStatus = BitField<u16, 5>;
        using ReadWriteSeekReady = BitField<u16, 6>;
        using DriveReady = BitField<u16, 7>;
        using SectorCounterOK = BitField<u16, 8>;
        using SeekIncomplete = BitField<u16, 9>;
        using DriveUnsafe = BitField<u16, 10>;
        using Rk05DiskOnLine = BitField<u16, 11>;
        using DrivePowerLow = BitField<u16, 12>;
        using DriveId = BitField<u16, 13, 3>;

        RKDS (u16 value = 0) : BasicRegister {value} {}

        // Copying a register copies its value; the fields of the copy refer
        // to the value of the copy.
        RKDS (RKDS const& other) : BasicRegister {other.value_} {}
        RKDS& operator= (RKDS const& other)
        {
            value_ = other.value_;
            return *this;
        }

        // Make the BasicRegister::operator= (u16) function visible to users
        // of this class. The copy assignment operator above hides
        // BasicRegister::operator= (u16). Without this using declaration
        // an assignment of a u16 value would still compile, but via the
        // converting constructor and the copy assignment instead of via
        // the Register interface.
        using BasicRegister::operator=;

        // The fields refer to (and do not copy) the register value
        RegisterField<SectorCounter> sectorCounter {value_};
        RegisterField<SectorCounterEqualsSectorAddress> sectorCounterEqualsSectorAddress {value_};
        RegisterField<WriteProtectStatus> writeProtectStatus {value_};
        RegisterField<ReadWriteSeekReady> readWriteSeekReady {value_};
        RegisterField<DriveReady> driveReady {value_};
        RegisterField<SectorCounterOK> sectorCounterOK {value_};
        RegisterField<SeekIncomplete> seekIncomplete {value_};
        RegisterField<DriveUnsafe> driveUnsafe {value_};
        RegisterField<Rk05DiskOnLine> rk05DiskOnLine {value_};
        RegisterField<DrivePowerLow> drivePowerLow {value_};
        RegisterField<DriveId> driveId {value_};
    };

    // Error register. This is a read-only register.
    // Error register. This is a read-only register.
    class RKER : public BasicRegister
    {
    public:
        using WriteCheckError = BitField<u16, 0>;
        using ChecksumError = BitField<u16, 1>;
        using Unused = BitField<u16, 2, 3>;
        using NonexistentSector = BitField<u16, 5>;
        using HardError = BitField<u16, 5, 11>;
        using NonexistentCylinder = BitField<u16, 6>;
        using NonExistentDisk = BitField<u16, 7>;
        using TimingError = BitField<u16, 8>;
        using DataLate = BitField<u16, 9>;
        using NonexistentMemory = BitField<u16, 10>;
        using ProgrammingError = BitField<u16, 11>;
        using SeekError = BitField<u16, 12>;
        using WriteLockoutViolation = BitField<u16, 13>;
        using Overrun = BitField<u16, 14>;
        using DriveError = BitField<u16, 15>;

        RKER (u16 value = 0) : BasicRegister {value} {}

        // Copying a register copies its value; the fields of the copy refer
        // to the value of the copy.
        RKER (RKER const& other) : BasicRegister {other.value_} {}
        RKER& operator= (RKER const& other)
        {
            value_ = other.value_;
            return *this;
        }

        // The copy assignment operator above hides BasicRegister::operator= (u16).
        // Without this using declaration an assignment of a u16 value would
        // still compile, but via the converting constructor and the copy
        // assignment instead of via the Register interface.
        using BasicRegister::operator=;

        // The fields refer to (and do not copy) the register value
        RegisterField<WriteCheckError> writeCheckError {value_};
        RegisterField<ChecksumError> checksumError {value_};
        RegisterField<Unused> unused {value_};
        RegisterField<NonexistentSector> nonexistentSector {value_};
        RegisterField<HardError> hardError {value_};
        RegisterField<NonexistentCylinder> nonexistentCylinder {value_};
        RegisterField<NonExistentDisk> nonExistentDisk {value_};
        RegisterField<TimingError> timingError {value_};
        RegisterField<DataLate> dataLate {value_};
        RegisterField<NonexistentMemory> nonexistentMemory {value_};
        RegisterField<ProgrammingError> programmingError {value_};
        RegisterField<SeekError> seekError {value_};
        RegisterField<WriteLockoutViolation> writeLockoutViolation {value_};
        RegisterField<Overrun> overrun {value_};
        RegisterField<DriveError> driveError {value_};
    };

    // Control Status register.
    class RKCS : public BasicRegister
    {
    public:
        using Go = BitField<u16, 0>;                        // Write Only
        using Operation = BitField<u16, 1, 3>;              // Read/Write
        using MemoryExtension = BitField<u16, 4, 2>;        // Read/Write
        using InterruptOnDoneEnable = BitField<u16, 6>;     // Read/Write
        using ControlReady = BitField<u16, 7>;              // Read Only
        using StopOnSoftError = BitField<u16, 8>;           // Read/Write
        using ExtraBit = BitField<u16, 9>;                  // Unused for RK11-D/E
        using Format = BitField<u16, 10>;                   // Read/Write
        using InhibitIncrementingRKBA = BitField<u16, 11>;  // Read/Write
        using Unused = BitField<u16, 12>;
        using SearchComplete = BitField<u16, 13>;           // Read Only
        using HardError = BitField<u16, 14>;                // Read Only
        using Error = BitField<u16, 15>;                    // Read Only

        RKCS (u16 value = 0) : BasicRegister {value} {}

        // Copying a register copies its value; the fields of the copy refer
        // to the value of the copy.
        RKCS (RKCS const& other) : BasicRegister {other.value_} {}
        RKCS& operator= (RKCS const& other)
        {
            value_ = other.value_;
            return *this;
        }

        // The copy assignment operator above hides BasicRegister::operator= (u16).
        // Without this using declaration an assignment of a u16 value would
        // still compile, but via the converting constructor and the copy
        // assignment instead of via the Register interface.
        using BasicRegister::operator=;

        // The fields refer to (and do not copy) the register value
        RegisterField<Go> go {value_};
        RegisterField<Operation> operation {value_};
        RegisterField<MemoryExtension> memoryExtension {value_};
        RegisterField<InterruptOnDoneEnable> interruptOnDoneEnable {value_};
        RegisterField<ControlReady> controlReady {value_};
        RegisterField<StopOnSoftError> stopOnSoftError {value_};
        RegisterField<ExtraBit> extraBit {value_};
        RegisterField<Format> format {value_};
        RegisterField<InhibitIncrementingRKBA> inhibitIncrementingRKBA {value_};
        RegisterField<Unused> unused {value_};
        RegisterField<SearchComplete> searchComplete {value_};
        RegisterField<HardError> hardError {value_};
        RegisterField<Error> error {value_};
    };

    // Disk Address register. This is a read/write register.
    // 
    // The RKDA is incremented automatically at the end of each disk sector
    // (EK-RK11D-MM-002, p. 3-8). To this end the register is equipped with
    // the assignment and addition operators.
    //
    // Disk Address register. This is a read/write register.
    //
    // The RKDA is incremented automatically at the end of each disk sector
    // (EK-RK11D-MM-002, p. 3-8). To this end the register is equipped with
    // the assignment and addition operators.
    //
    class RKDA : public BasicRegister
    {
    public:
        using SectorAddress = BitField<u16, 0, 4>;
        using Surface = BitField<u16, 4>;
        using CylinderAddress = BitField<u16, 5, 8>;
        using DriveSelect = BitField<u16, 13, 3>;

        RKDA (u16 value = 0) : BasicRegister {value} {}

        // Copying a register copies its value; the fields of the copy refer
        // to the value of the copy.
        RKDA (RKDA const& other) : BasicRegister {other.value_} {}
        RKDA& operator= (RKDA const& other)
        {
            value_ = other.value_;
            return *this;
        }

        // The copy assignment operator above hides BasicRegister::operator= (u16).
        // Without this using declaration an assignment of a u16 value would
        // still compile, but via the converting constructor and the copy
        // assignment instead of via the Register interface.
        using BasicRegister::operator=;

        // Set the disk address, leaving the drive select unchanged
        RKDA& operator= (DiskAddress const& diskAddress)
        {
            sectorAddress = diskAddress.sector;
            surface = diskAddress.head;
            cylinderAddress = diskAddress.cylinder;
            return *this;
        }

        // Return the disk address numSectors further on; the drive select
        // is copied unchanged. The parameter is an int to avoid ambiguity
        // with the built-in operator+ via the conversion to u16.
        RKDA operator+ (int numSectors) const
        {
            RKDA result {*this};
            result = rk05Geometry_.lbnTodiskAddress (
                rk05Geometry_.LBN (*this) + numSectors);
            return result;
        }

        RKDA& operator+= (int numSectors)
        {
            return *this = *this + numSectors;
        }

        operator DiskAddress () const
        {
            return DiskAddress {sectorAddress, surface, cylinderAddress};
        }

        // The fields refer to (and do not copy) the register value
        RegisterField<SectorAddress> sectorAddress {value_};
        RegisterField<Surface> surface {value_};
        RegisterField<CylinderAddress> cylinderAddress {value_};
        RegisterField<DriveSelect> driveSelect {value_};
    };

    // Definition of the RK11-D Operations
    // 
    // Disclaimer: use of an enum instead of an enum class as the enum class
    // makes comparing with u16 values cumbersome, even if the enum class is
    // derived from u16.
    //
    enum Operation
    {
        ControlReset,
        Write,
        Read,
        WriteCheck,
        Seek,
        ReadCheck,
        DriveReset,
        WriteLock
    };

    // Definition of the function to be processed by the RK11D action processor
    struct Function
    {
        RKCS rkcs;
        RKDA diskAddress;
        u16 wordCount;
        BusAddress busAddress;
    };

    // Definition of a seek complete report by a RK05 drive
    struct SeekCompleteReport
    {
        u16 driveId;
        RKER rker;
    };

    // The completed execution of a command is reported by the number of
    // words transferred and the number of sectors processed.
    struct FunctionResult
    {
        StatusCode statusCode {};
        size_t wordsTransferred {0};
        size_t sectorsProcessed {0};
    };
};

#endif // _RKTYPES_H_
