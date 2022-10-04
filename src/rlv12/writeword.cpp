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
StatusCode RLV12::writeByte (u16 registerAddress, u8 data)
{
    return StatusCode::NonExistingMemory;
}

StatusCode RLV12::writeWord (u16 registerAddress, u16 data)
{
    // Get reference to drive
    RL01_2 &unit = units_[GET_DRIVE(data)];
    std::shared_ptr<RLV12Command> rlv12Command;

    // Guard against controller register access from the command processor
	std::unique_lock<std::mutex> lock {controllerMutex_};

    // Decode registerAddress<3:1>
    switch (registerAddress & 016)
    {
        case CSR:
            // Control/Status Register
            // The next statement is superfluous as the BA16 ans BA17 bits
            // are immediately overwritten by the next statement
            // rlcs = (rlcs & ~RLCS_MEX) | ((rlbae & RLCS_M_MEX) << RLCS_V_MEX);

            rlcs = (rlcs & ~RLCS_RW) | (data & RLCS_RW);

            // Load Bus Address Extension Bits (BA16 and BA17) into bits
            // 00 and 01 in the BAE register
            // rlbae = (rlbae & ~RLCS_M_MEX) | ((rlcs >> RLCS_V_MEX) & RLCS_M_MEX);
            updateBAE ();

            TRACERLV12Registers (&trc, "write CSR", rlcs, rlba, rlda, rlmpr, rlbae);

            // Commands to the controller are only executed with the CRDY (DONE)
            // bit is cleared by software.  If set, check for interrupts and return.
            if (data & CSR_DONE)
            {                              
                // Ready set?
                if ((data & CSR_IE) == 0)
                    bus->clearInterrupt (TrapPriority::BR4, 4);
                else if ((rlcs & (CSR_DONE + CSR_IE)) == CSR_DONE)
                    bus->setInterrupt (TrapPriority::BR4, 4, vector_);

                return StatusCode::OK;
            }

            bus->clearInterrupt (TrapPriority::BR4, 4);

            // At the start of every command errors are cleared. Note that
            // Composite Error is cleared (or set) in read(), based on the
            // setting of the other error bits.
            rlcs &= ~RLCS_ALLERR;                   

            TRACERLV12Command (&trc, GET_FUNC(rlcs));

            // ToDo: Remove unit.function_
            // The function to be executed is coded in the created RLV12command
            unit.function_ = GET_FUNC(rlcs);

            // We're done using the registers this call. Notify the command
            // processor a command has been issued.
            lock.unlock ();
            signal.notify_one ();
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
            rlba = data & (rlType_ == 
                RlConfig::RLType::RL11 ? 0177776 : 0177777);
            break;

        case DAR:
            // Disk Address Register     
            rlda = data;
            break;

        case MPR:
            // Multipurpose Register
            rlmpr = rlmpr1 = rlmpr2 = (u16) data;
            break;

        case BAE:
            // Bus Address Extension Register
            // Not present in RL11/RLV11 and on an RLV12 with the 22-bit
            // option disabled.
            if (!(rlType_ == RlConfig::RLType::RLV12 && _22bit_))
                return StatusCode::NonExistingMemory;

            if (registerAddress & 1)
                return StatusCode::OK;

            rlbae = data & RLBAE_IMP;

            // Load bits 00 and 01 (BA16 and BA17) into the corresponding
            // bits in the CSR.
            // ToDo: This is an undocumented feature?
            rlcs = (rlcs & ~RLCS_MEX) | ((rlbae & RLCS_M_MEX) << RLCS_V_MEX);
            break;

        default:
            return StatusCode::NonExistingMemory;
    }                                           

    return StatusCode::OK;
}