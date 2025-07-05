#include "rk11d.h"
#include "trace/trace.h"

StatusCode RK11D::writeWord (BusAddress busAddress, u16 value)
{
    // Guard against controller register access from RK05 threads
    std::lock_guard<std::mutex> guard {controllerMutex_};

    // Decode registerAddress<3:1>
    switch (busAddress.registerAddress () & 016)
    {
        case RKDS:
            // Drive Status register
            break;

        case RKER:
            // Error register
            break;

        case RKCS:
            // Control Status register
            rkcs_.value = (rkcs_.value & ~rkcsWritableBits) |
                value & rkcsWritableBits;

            if (rkcs_.go)
            {
                startFunction ();

                // The controller is ready to accept a new command
                // According to EK-RK11D-MM-002 p. 3-4, the GO bit remains set
                // until the control actually begins to respond to GO, which may
                // take from 1 microsecond to 3.3 milliseconds, depending on the
                // current operation of the selected drive.
                rkcs_.go = 0;
            }
            break;

        case RKWC:
            // Word Count register
            rkwc_ = value;
            break;

        case RKBA:
            // Current Bus Address register
            rkba_ = value;
            break;

        case RKDA:
            // Disk Address register
            // The drive selection logic selects one of eight possible disk drives
            // on DR BUS from either the programmable RKDA 13—15 or the hardware
            // poll logic. (EK-RK11D-MM-002, par. 4.3.4)
            rkda_.value = value;
            selectedDrive_ = rkda_.driveSelect;
            break;

        case RKMR:
            // Maintenance register. Write's to it are ignored
            break;

        case RKDB:
            // Data Buffer register
            break;

        default:
            return (StatusCode::NonExistingMemory);
    }

    trace.rk11Registers (busAddress, rker_, getDriveStatus (selectedDrive_),
        rkcs_, rkwc_, rkba_, rkda_, rkdb_);

    return StatusCode::Success;
}

void RK11D::startFunction ()
{
    // The Control Ready bit is to be cleared by GO being set
    // (EK-RK11D-MM-002 p. 3-6)
    rkcs_.controlReady = 0;

    // Disclaimer: the u16 resulting from the BitField conversion
    // operator cannot be cast directly to an Function enum.
    u16 operation = rkcs_.operation;

    // Except for the Control Reset function a valid drive has
    // to be selected.
    if (selectedDrive_ >= rk05Drives_.size () &&
        operation != RKTypes::Operation::ControlReset)
    {
        setNonExistingDisk ();
        setControlReady ();
        return;
    }

    // For functions that are to be processed by the drive,
    // Drive Ready has to be cleared in the CPU thread as the
    // running program might check the status of that bit
    // immediately following setting of the the GO bit.
    if (operation != RKTypes::Operation::ControlReset)
        rk05Drives_[selectedDrive_]->clearDriveReady ();

    // ToDo: Add Memory Extension bits to bus address
    functionQueue_.push (RKTypes::Function
        {
            static_cast<RKTypes::Operation> (operation),
            rkda_.value, rkwc_, rkba_
        });

    functionAvailable_.notify_one ();
}