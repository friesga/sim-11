#include "rlv12.h"

#include "asynctimer/asynctimer.h"

#include <chrono>

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

            TRACERLV12Registers (&trc, "writeWord", rlcs, rlba, rlda, rlmpr, rlbae); 

            // Commands to the controller are only executed with the CRDY (DONE)
            // bit is cleared by software.  If set, check for interrupts and return.
            if (data & CSR_DONE)
            {                              
                // Ready set?
                if ((data & CSR_IE) == 0)
                    bus->clearInterrupt (TrapPriority::BR4, 2);
                else if ((rlcs & (CSR_DONE + CSR_IE)) == CSR_DONE)
                    bus->setInterrupt (TrapPriority::BR4, 2, vector_);

                return StatusCode::OK;
            }

            bus->clearInterrupt (TrapPriority::BR4, 2);

            // Clear errors
            rlcs &= ~RLCS_ALLERR;                   

            TRACERLV12Command (&trc, GET_FUNC(rlcs));
            switch (GET_FUNC(rlcs))
            {
                // Case on RLCS<3:1>
                case RLCS_NOP:
                    // NOP on the RL11 controller and Maintenance Mode on
                    // the RLV1[12]
                    // ToDo: The RL type test should be done on controller type
                    if (!UNIBUS)
                        maintenance ();
                    setDone (0);
                    break;

                case RLCS_SEEK:
                    seek (unit);
                    break;

                case RLCS_GSTA:
                    // Get Status Command
                    getStatus (unit);
                    break;

                default:
                    // Data transfer commands:
                    // Write Check (Function Code 1),
                    // Read Header (Function Code 4),
                    // Write Data (Function Code 5),
                    // Read Data (Function Code 6),
                    // Read Data without Header Check (Function Code 7)
                    if (unit.unitStatus_ & Status::UNIT_DIS || 
                        unit.rlStatus_ & RlStatus::UNIT_OFFL || 
                        !(unit.unitStatus_ & Status::UNIT_ATT))
                    {
                        TRACERLV12Registers (&trc, "Command incomplete", 
                            rlcs, rlba, rlda, rlmpr, rlbae); 
                        setDone (RLCS_INCMP);
                        break;
                    }

                    /*
                    EK-0RL11-TD-001, p2-3: "If the CPU software initiates another
                    operation on a drive that is busy seeking, the controller will
                    suspend the operation until the seek is completed."

                    Check for the condition where there is an outstanding operation but
                    the program is requesting another operation without waiting for
                    drive ready.  If so, remove the previous queue entry, complete the
                    operation now, and queue the next operation.
                    */
                    if (timer.isRunning (&unit))
                    {
                        timer.cancel (&unit);
                        unit ();
                    }

                    unit.function_ = GET_FUNC(rlcs);

                    // Activate unit
                    timer.start (&unit, std::chrono::milliseconds (rl_swait));              
                    break;
            }           
            break;

            /*
            Contrary to what the RL01/RL02 User Guide (EK-RL012-UG-006, p.4-5)
            says, bit 0 can be written and read (as 1) on an RLV12 (verified
            2011-01-05).  Not sure about the RLV11.
            */
        case BAR:
            // Bus Address Register
            rlba = data & (UNIBUS ? 0177776 : 0177777);
            // if (DEBUG_PRS(rl_dev))
            //    fprintf(sim_deb, ">>RL wr: RLBA %06o\n", rlba);
            break;

        case DAR:
            // Disk Address Register     
            rlda = data;
            // if (DEBUG_PRS(rl_dev))
            //    fprintf(sim_deb, ">>RL wr: RLDA %06o\n", rlda);
            break;

        case MPR:
            // Multipurpose Register
            rlmpr = rlmpr1 = rlmpr2 = (u16) data;
            // if (DEBUG_PRS(rl_dev))
            //    fprintf(sim_deb, ">>RL wr: RLMP %06o\n", rlmp);
            break;

        case BAE:
            // Bus Address Extension Register
            if (UNIBUS || (flags_ & DEV_RLV11))       /* not in RL11/RLV11 */
                return StatusCode::NonExistingMemory;

            if (registerAddress & 1)
                return StatusCode::OK;

            rlbae = data & RLBAE_IMP;

            // Load bits 00 and 01 (BA16 and BA17) into the corresponding
            // bits in the CSR.
            // ToDo: This is an undocumented feature?
            rlcs = (rlcs & ~RLCS_MEX) | ((rlbae & RLCS_M_MEX) << RLCS_V_MEX);

            //if (DEBUG_PRS(rl_dev))
            //    fprintf(sim_deb, ">>RL wr: RLBAE %06o\n", rlbae);
            break;

        default:
            return StatusCode::NonExistingMemory;
    }                                           

    return StatusCode::OK;
}