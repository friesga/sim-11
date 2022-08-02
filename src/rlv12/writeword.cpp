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

    int32_t currentCylinder;
    int32_t offset;
    int32_t newCylinder;
    int32_t maxCylinder, tim;

    // Get reference to drive
    RL01_2 &unit = units_[GET_DRIVE(data)];

    // Decode registerAddress<3:1>
    switch (registerAddress & 016)
    {
        case CSR:
            // Control/Status Register
            rlcs = (rlcs & ~RLCS_MEX) | ((rlbae & RLCS_M_MEX) << RLCS_V_MEX);

            rlcs = (rlcs & ~RLCS_RW) | (data & RLCS_RW);
            rlbae = (rlbae & ~RLCS_M_MEX) | ((rlcs >> RLCS_V_MEX) & RLCS_M_MEX);

            if(trc.flags)
                TRACERLV12Registers (&trc, "writeword", rlcs, rlba, rlda, rlmpr, rlbae); 

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
                    // Seek function
                    if ((unit.flags_ & (UNIT_DIS | UNIT_OFFL)) ||
                        (!(unit.flags_ & UNIT_ATT)))
                    {
                        // No cartridge available to perform a seek on
                        setDone (RLCS_ERR | RLCS_INCMP);
                        unit.status_ |= RLDS_STO;
                        break;
                    }

                    currentCylinder = getCylinder (unit.currentTrack_);
                    offset = getCylinder (rlda);

                    // Seek direction in or out?
                    if (rlda & RLDA_SK_DIR)
                    {
                        // Outwards
                        newCylinder = currentCylinder + offset;
                        maxCylinder = (unit.flags_ & UNIT_RL02) ? RL_NUMCY * 2 : RL_NUMCY;
                        if (newCylinder >= maxCylinder)
                            newCylinder = maxCylinder - 1;
                    }
                    else
                    {
                        // Inwards
                        newCylinder = currentCylinder - offset;
                        if (newCylinder < 0)
                            newCylinder = 0;
                    }

                    // Enter velocity mode? Only if a different cylinder
                    if (newCylinder != currentCylinder)
                        // Move the positioner 
                        unit.status_ = (unit.status_ & ~RLDS_M_STATE) | RLDS_SEEK; 

                    // ToDo: If a head switch, sector should be RL_NUMSC/2?
                    // Put on track
                    unit.currentTrack_ = (newCylinder << RLDA_V_CYL) |      
                        ((rlda & RLDA_SK_HD) ? RLDA_HD1 : RLDA_HD0);

                    // Real timing:
                    // min 6.5ms, max 15ms for head switch,
                    // max 17ms for 1 track seek w/head switch
                    // 55ms avg seek
                    // 100ms max seek

                    tim = abs (newCylinder - currentCylinder);
                    if (tim == 0)
                        tim++;

                    tim *= rl_swait;
                    // if (DEBUG_PRS(rl_dev))
                    //     fprintf(sim_deb, ">>RL SEEK: drv %d, dist %d, head sw %d, tim %d\n",
                    //        (int32)(uptr - rl_dev.units),
                    //         abs(newCylinder - currentCylinder), (rlda & RLDA_SK_HD), tim);
                    // ToDo: Change RLCS_ macro's to enum
                    unit.function_ = RLCS_SEEK;
                    // sim_activate(uptr, tim);               /* must be > 0 */
                    timer.start (&unit, std::chrono::milliseconds (tim));
                    setDone (0);
                    break;

                case RLCS_GSTA:
                    // Get Status Command
                    getStatus (unit);
                    break;

                default:
                    // Data transfer
                    if ((unit.flags_ & (UNIT_DIS | UNIT_OFFL)) || (!(unit.flags_ & UNIT_ATT)))
                    {
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
            rlcs = (rlcs & ~RLCS_MEX) | ((rlbae & RLCS_M_MEX) << RLCS_V_MEX);
            //if (DEBUG_PRS(rl_dev))
            //    fprintf(sim_deb, ">>RL wr: RLBAE %06o\n", rlbae);
            break;

        default:
            return StatusCode::NonExistingMemory;
    }                                           

    return StatusCode::OK;
}