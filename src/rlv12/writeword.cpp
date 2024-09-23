#include "rlv12.h"
#include "threadsafecontainers/threadsafequeue.h"

#include <chrono>
#include <mutex>

using std::mutex;
using std::lock_guard;

// The RLV12 registers are word-addressable (see RLV12 Disk Controller User's
// Guide, EK-RLV12-UG-002) and DATOB cycles are not recognized. The 
// controller's behaviour on a register byte access is undefined. To have some
// sort of orderly reaction we'll generate a bus timeout (NonExistingMemory).
// See also:
// https://retrocomputing.stackexchange.com/questions/24823/dec-rlv12-disk-controller-register-access
//
StatusCode RLV12::writeByte (BusAddress busAddress, u8 data)
{
    return StatusCode::NonExistingMemory;
}

StatusCode RLV12::writeWord (BusAddress busAddress, u16 data)
{
    // Get reference to drive
    RL01_02 &unit = units_[RLV12const::getDrive (data)];

    // Guard against controller register access from the command processor
	std::unique_lock<std::mutex> lock {controllerMutex_};

    // Any write to a register resets the FIFO buffer index
    fifoIndex_ = 0;

    // Decode registerAddress<3:1>
    switch (busAddress.registerAddress () & 016)
    {
        case CSR:
            // Control/Status Register
            // The next statement is superfluous as the BA16 ans BA17 bits
            // are immediately overwritten by the next statement
            // rlcs = (rlcs & ~RLCS_MEX) | ((bae_ & RLCS_M_MEX) << RLCS_V_MEX);

            // Set the writable bits in the CSR. CRDY will be cleared by the
            // host software to execute the command specified in the CSR.
            csr_ = (csr_ & ~RLV12const::CSR_ReadWriteBits) | (data & RLV12const::CSR_ReadWriteBits);

            // The [DRDY] bit is cleared when a seek or head-select operation
            // is initiated [...] (EK-RLV12-TD-001, Table 3-3).
            // The DRDY bit has to be cleared at this point as we cannot
            // guarantee it will be cleared by the command processor before
            // the CSR is read by the host software.
            if (RLV12const::getFunction (csr_) == RLV12const::CSR_Seek)
                unit.driveStatus_ = 
                    (unit.driveStatus_ & ~RLV12const::MPR_GS_State) | RLV12const::MPR_GS_Seek;

            // Load Bus Address Extension Bits (BA16 and BA17) into bits
            // 00 and 01 in the BAE register
            updateBAE ();

            trace.rlv12Registers ("write CSR", csr_, bar_, dar_, 
                dataBuffer_[0], bae_);

            // Commands to the controller are only executed with the CRDY (DONE)
            // bit is cleared by software.  If set, check for interrupts and return.
            if (data & RLV12const::CSR_ControllerReady)
            {                              
                // Ready set?
                if ((data & RLV12const::CSR_InterruptEnable) == 0)
                    bus_->clearInterrupt (TrapPriority::BR4, 0, 4);
                else if ((csr_ & (RLV12const::CSR_ControllerReady + RLV12const::CSR_InterruptEnable))
                        == RLV12const::CSR_ControllerReady)
                    bus_->setInterrupt (TrapPriority::BR4, 4, 0, vector_);

                return StatusCode::OK;
            }

            bus_->clearInterrupt (TrapPriority::BR4, 0, 4);

            // At the start of every command errors are cleared. Note that
            // Composite Error is cleared (or set) in read(), based on the
            // setting of the other error bits.
            csr_ &= ~RLV12const::CSR_AnyError;

            trace.rlv12Command (RLV12const::getFunction (csr_));

            // Diagnostic ZRLJC0 requires a response from the Get Status
            // command within 10 microseconds. As a context switch to the
            // command processor occasionally leads to the exceeding of this
            // value, the Get Status command is executed in the current
            // context.
            if (RLV12const::getFunction (csr_) == RLV12const::CSR_GetStatus)
            {
                setDone (unit, getStatusCmd (&units_[RLV12const::getDrive (csr_)]));
                break;
            }

            // We're done using the registers this call. Notify the
            // command processor a command has been issued.
            lock.unlock ();
            signal_.notify_one ();
            break;

        case BAR:
            // Bus Address Register
            // Contrary to what the RL01/RL02 User Guide (EK-RL012-UG-006, 
            // p.4-5) says, bit 0 can be written and read (as 1) on an RLV12
            // (verified 2011-01-05).  Not sure about the RLV11. 
            // Source: simh comment
            //
            // The VRLBC0 diagnostics makes clear that bit 0 can be read and
            // written on the RLV11 and RLV12 and always reads as 0 on an RL11.
            bar_ = data & (rlType_ == 
                RLConfig::RLType::RL11 ? 0177776 : 0177777);
            break;

        case DAR:
            // Disk Address Register     
            dar_ = data;
            break;

        case MPR:
            // Multipurpose Register
            wordCounter_ = data;
            break;

        case BAE:
            // Bus Address Extension Register
            // Not present in RL11/RLV11 and on an RLV12 with the 22-bit
            // option disabled.
            if (!(rlType_ == RLConfig::RLType::RLV12 && _22bit_))
                return StatusCode::NonExistingMemory;

            if (busAddress.registerAddress () & 1)
                return StatusCode::OK;

            bae_ = data & RLV12const::BAE_Mask;

            // Load bits 00 and 01 (BA16 and BA17) into the corresponding
            // bits in the CSR.
            // ToDo: This is an undocumented feature?
            // rlcs = (rlcs & ~RLCS_MEX) | ((bae_ & RLCS_M_MEX) << RLCS_V_MEX);
            csr_ = (csr_ & ~RLV12const::CSR_AddressExtension) |
                    ((bae_ & RLV12const::CSR_AddressExtMask) << RLV12const::CSR_AddressExtPosition);
            break;

        default:
            return StatusCode::NonExistingMemory;
    }                                           

    return StatusCode::OK;
}