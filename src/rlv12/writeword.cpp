#include "rlv12.h"

// The RLV12 registers are word-addressable (see RLV12 Disk Controller User's
// Guide, EK-RLV12-UG-002) and DATOB cycles are not recognized. The 
// controller's behaviour on a register byte access is undefined. To have some
// sort of orderly reaction We'll generate a bus timeout (NonExistingMemory).
// See also:
// https://retrocomputing.stackexchange.com/questions/24823/dec-rlv12-disk-controller-register-access
//
StatusCode RLV12::writeByte (u16 registerAddress, u8 data)
{
    return StatusCode::NonExistingMemory;
}

StatusCode RLV12::writeWord (u16 registerAddress, u16 data)
{
#if 0
    int32_t curr, offs, newc, maxc, tim;
    UNIT* uptr;

    switch (registerAddress & 06)
    {
        // Decode registerAddress<2:1>
        case CSR:
            // Control/Status Register
            rlcs = (rlcs & ~RLCS_MEX) | ((rlbae & RLCS_M_MEX) << RLCS_V_MEX);

            // Get reference to new drive
            RL01_2 &unit = units_[GET_DRIVE(data)];

            rlcs = (rlcs & ~RLCS_RW) | (data & RLCS_RW);
            rlbae = (rlbae & ~RLCS_M_MEX) | ((rlcs >> RLCS_V_MEX) & RLCS_M_MEX);

            // ToDo: Add state to TRACERLV12Registers
            if(trc.flags)
                TRACERLV12Registers (&trc, rlcs, rlba, rlda, rlmpr, rlbae); 

            // Commands to the controller are only executed with the CRDY (DONE)
            // bit is cleared by software.  If set, check for interrupts and return.
            if (data & CSR_DONE)
            {                              
                // Ready set?
                if ((data & CSR_IE) == 0)
                    CLR_INT(RL);
                else if ((rlcs & (CSR_DONE + CSR_IE)) == CSR_DONE)
                    SET_INT(RL);
                return StatusCode::OK;
            }

            CLR_INT(RL);                                   /* clear interrupt */
            rlcs &= ~RLCS_ALLERR;                           /* clear errors */
            switch (GET_FUNC(rlcs))
            {                      /* case on RLCS<3:1> */
                case RLCS_NOP:                                  /* nop */
                    if (!UNIBUS)                                /* RLV1x has MAINT command */
                        rlv_maint();
                    rl_set_done(0);
                    break;
                case RLCS_SEEK:                                 /* seek */
                    if ((uptr->flags & (UNIT_DIS | UNIT_OFFL)) || (!(uptr->flags & UNIT_ATT)))
                    {
                        rl_set_done(RLCS_ERR | RLCS_INCMP);
                        uptr->STAT |= RLDS_STO;
                        break;
                    }
                    curr = GET_CYL(uptr->TRK);             /* current cylinder */
                    offs = GET_CYL(rlda);                  /* offset */
                    if (rlda & RLDA_SK_DIR)
                    {               /* in or out? */
                        newc = curr + offs;                 /* out */
                        maxc = (uptr->flags & UNIT_RL02) ?
                            RL_NUMCY * 2 : RL_NUMCY;
                        if (newc >= maxc)
                            newc = maxc - 1;
                    }
                    else
                    {
                        newc = curr - offs;                 /* in */
                        if (newc < 0)
                            newc = 0;
                    }
                    /* enter velocity mode? only if a different cylinder */
                    if (newc != curr)
                        uptr->STAT = (uptr->STAT & ~RLDS_M_STATE) | RLDS_SEEK; /* move the positioner */
            /* TBD: if a head switch, sector should be RL_NUMSC/2? */
                    uptr->TRK = (newc << RLDA_V_CYL) |      /* put on track */
                        ((rlda & RLDA_SK_HD) ? RLDA_HD1 : RLDA_HD0);
                    /*
                    Real timing:
                    min 6.5ms, max 15ms for head switch,
                    max 17ms for 1 track seek w/head switch
                    55ms avg seek
                    100ms max seek
                    */
                    tim = abs(newc - curr);
                    if (tim == 0)
                        tim++;
                    tim *= rl_swait;
                    if (DEBUG_PRS(rl_dev))
                        fprintf(sim_deb, ">>RL SEEK: drv %d, dist %d, head sw %d, tim %d\n",
                            (int32)(uptr - rl_dev.units),
                            abs(newc - curr), (rlda & RLDA_SK_HD), tim);
                    uptr->FNC = RLCS_SEEK;
                    sim_activate(uptr, tim);               /* must be > 0 */
                    rl_set_done(0);                        /* ctrlr is ready */
                    break;
                case RLCS_GSTA:
                    if (!(rlda & RLDA_GS))
                    {                /* GS bit must be set */
                        rl_set_done(RLCS_ERR | RLCS_INCMP);    /* OPI; request error */
                        return (SCPE_OK);
                    }
                    if (rlda & RLDA_GS_CLR)                 /* reset errors? */
                        uptr->STAT &= ~RLDS_ERR;
                    /* develop drive state */
                    rlmp = (uint16)(uptr->STAT | (uptr->TRK & RLDS_HD));
                    if (uptr->flags & UNIT_RL02)
                        rlmp |= RLDS_RL02;
                    if (uptr->flags & UNIT_WPRT)
                        rlmp |= RLDS_WLK;
                    if (uptr->flags & (UNIT_DIS | UNIT_OFFL))
                    {
                        rlmp |= RLDS_DSE;
                        rl_set_done(RLCS_DRE | RLCS_INCMP);
                    }
                    rlmp2 = rlmp1 = rlmp;
                    if (DEBUG_PRS(rl_dev))
                        fprintf(sim_deb, ">>RL GSTA: rlds=%06o drv=%ld\n",
                            rlmp, (long)(uptr - rl_dev.units));
                    rl_set_done(0);                        /* done */
                    break;
                default:                                    /* data transfer */
                    if ((uptr->flags & (UNIT_DIS | UNIT_OFFL)) || (!(uptr->flags & UNIT_ATT)))
                    {
                        rl_set_done(RLCS_INCMP);
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
                    if (sim_is_active(uptr))
                    {
                        sim_cancel(uptr);
                        rl_svc(uptr);
                    }
                    uptr->FNC = GET_FUNC(rlcs);
                    sim_activate(uptr, rl_swait);              /* activate unit */
                    break;
            }                                           /* end switch func */
            break;                                          /* end case RLCS */
    /*
    Contrary to what the RL01/RL02 User Guide (EK-RL012-UG-006, p.4-5)
    says, bit 0 can be written and read (as 1) on an RLV12 (verified
    2011-01-05).  Not sure about the RLV11.
    */
        case BAR:
            // Bus Address Register
            rlba = data & (UNIBUS ? 0177776 : 0177777);
            if (DEBUG_PRS(rl_dev))
                fprintf(sim_deb, ">>RL wr: RLBA %06o\n", rlba);
            break;

        case DAR:
            // Disk Address Register
            
            rlda = data;
            if (DEBUG_PRS(rl_dev))
                fprintf(sim_deb, ">>RL wr: RLDA %06o\n", rlda);
            break;

        case MPR:
            // Multipurpose Register
            
            rlmp = rlmp1 = rlmp2 = (uint16)data;
            if (DEBUG_PRS(rl_dev))
                fprintf(sim_deb, ">>RL wr: RLMP %06o\n", rlmp);
            break;

        case BAE:
            // Bus Address Extension Register
            if (UNIBUS || (rl_dev.flags & DEV_RLV11))       /* not in RL11/RLV11 */
                return SCPE_NXM;
            if (registerAddress & 1)
                return SCPE_OK;
            rlbae = data & RLBAE_IMP;
            rlcs = (rlcs & ~RLCS_MEX) | ((rlbae & RLCS_M_MEX) << RLCS_V_MEX);
            if (DEBUG_PRS(rl_dev))
                fprintf(sim_deb, ">>RL wr: RLBAE %06o\n", rlbae);
            break;

        default:
            return (SCPE_NXM);
    }                                               /* end switch */
#endif
    return StatusCode::OK;
}