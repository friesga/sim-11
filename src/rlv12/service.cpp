#include "rlv12.h"

//
// Service unit timeout
//
//  If seek in progress, complete seek command
//  Else complete data transfer command
//
// The unit control block contains the function and cylinder for
// the current command.
//
// This function services a timer expiration.
// 
// The function originally returned a status code which was then processed
// in the main instruction loop. To maintain encapsulation RLV12 conditions
// are to be handled in the RLV12 class.
//
void RLV12::service (Unit &unitRef)
{

    int32_t err, wc, maxwc, t;
    int32_t i, da, awc;
    uint32_t ma;
    RL01_2 &unit = static_cast<RL01_2&> (unitRef);
 
    const char* const funcname[] = {
        "NOP", "WCK", "GSTA", "SEEK",
        "RHDR", "WT", "RD", "RNOHDR", "SPECIAL",
    };

    // if (DEBUG_PRS(rl_dev))
    //{
    //    if (unit.function_ == RLCS_SPECIAL)
    //        fprintf(sim_deb, ">>RL svc: func=SPECIAL(%s) drv=%d\n",
    //            state[unit.status_ & RLDS_M_STATE], (int32)(uptr - rl_dev.units));
    //    else
    //        fprintf(sim_deb, ">>RL svc: func=%s drv=%d rlda=%06o\n",
    //            funcname[unit.function_], (int32)(uptr - rl_dev.units), rlda);
    //}

    // Really shouldn't happen...
    if ((unit.function_ == RLCS_GSTA) || (unit.function_ == RLCS_NOP))
    {
        setDone (0);
        return;
    }

    // This situation occurs when the drive (not controller) state needs
    // to transition from one state to another.  The state bits indicate
    // the state the drive is currently in.

    if (unit.function_ == RLCS_SPECIAL)
    {
        switch (unit.status_ & RLDS_M_STATE)
        {
            // The LOAD state is a little different.  We can stay in LOAD until
            // the user hits the RUN (LOAD) button, at which time we should come
            // here to transition to the next state and begin the startup process.
            case RLDS_LOAD:
                // Load pressed, spinning up 
                if (!(unit.status_ & RLDS_CVO))
                {
                    unit.status_ = (unit.status_ & ~RLDS_M_STATE) | RLDS_SPIN;
                    // Actual time is 45-50 seconds from press to Lock 
                    timer.start (&unit, std::chrono::milliseconds (200 * rl_swait));
                    
                    unit.status_ &= ~RLDS_HDO;
                    unit.status_ |= RLDS_BHO;
                }
                break;
                
                // Original RL01 drives would transition to the Brush Cycle, but this
                // was removed in a later ECO.
                
            case RLDS_SPIN:     
                // Spun up, load brushes or heads
                if (unit.flags_ & UNIT_BRUSH)
                {
                    unit.status_ &= ~RLDS_BHO;
                    unit.status_ = (unit.status_ & ~RLDS_M_STATE) | RLDS_BRUSH;
                }
                else
                {
                    unit.status_ |= RLDS_BHO;
                    unit.status_ = (unit.status_ & ~RLDS_M_STATE) | RLDS_HLOAD;
                }
                timer.start (&unit, std::chrono::milliseconds (200 * rl_swait));
                break;

            case RLDS_BRUSH:
                unit.status_ = (unit.status_ & ~RLDS_M_STATE) | RLDS_HLOAD;
                unit.status_ |= RLDS_BHO;
                timer.start (&unit, std::chrono::milliseconds (200 * rl_swait));
                break;

            case RLDS_HLOAD:
                // Heads loaded, seek to home 
                unit.status_ = (unit.status_ & ~RLDS_M_STATE) | RLDS_SEEK;
                timer.start (&unit, std::chrono::milliseconds (200 * rl_swait));
                unit.status_ |= RLDS_BHO | RLDS_HDO;
                unit.currentTrack_ = 0;
                break;

            case RLDS_SEEK:
                // Home found, lock on 
                // Enter postion mode
                unit.status_ = (unit.status_ & ~RLDS_M_STATE) | RLDS_LOCK;
                // Commented out in simhv312-2 version:
                // sim_activate (uptr, rl_swait);
                break;

            case RLDS_LOCK:     
                // Tracking, nothing to do
                // Illuminate ready lamp
                break;

            case RLDS_UNL:
                // Initiated by depressing the Run (LOAD) switch.
                // Unload pressed, heads unloaded, spin down
                unit.status_ = (unit.status_ & ~RLDS_M_STATE) | RLDS_DOWN;

                // Retract heads
                unit.status_ &= ~RLDS_HDO;    

                // Actual time is ~30 seconds
                timer.start (&unit, std::chrono::milliseconds (200 * rl_swait));
                break;

            case RLDS_DOWN:
                // OK to open cover
                unit.status_ = (unit.status_ & ~RLDS_M_STATE) | RLDS_LOAD;
                unit.status_ |= RLDS_BHO | RLDS_VCK;
                break;

            default:
                // Can't happen
                break; 
        }
        return;
    }

    // Attached?
    if ((unit.flags_ & UNIT_ATT) == 0)
    {
        // Spin error
        unit.status_ |= RLDS_SPE;                       

        // Flag error
        setDone (RLCS_ERR | RLCS_INCMP);                
        // ToDo: return IORETURN(rl_stopioe, SCPE_UNATT); ?
        return;
    }

    if ((unit.function_ == RLCS_WRITE) && (unit.flags_ & UNIT_WPRT))
    {
        // Write and locked
        unit.status_ |= RLDS_WGE;                     
        setDone (RLCS_ERR | RLCS_DRE);
        return;
    }

    if (unit.function_ == RLCS_SEEK)
    {
        // Seek?
        // Enter position mode
        // Heads locked on cyl
        unit.status_ = (unit.status_ & ~RLDS_M_STATE) | RLDS_LOCK; 
        return;
    }

    if (unit.function_ == RLCS_RHDR)
    {
        // Read header?
        u16 hdr[2];

        hdr[0] = rlmpr = unit.currentTrack_ & 0177777;
        hdr[1] = rlmpr1 = 0;
        // Calculate header CRC
        rlmpr2 = calcCRC(2, &hdr[0]);
        setDone (0);

        // Simulate sequential rotation about the current track
        unit.currentTrack_ = (unit.currentTrack_ & ~RLDA_M_SECT) |
            ((unit.currentTrack_ + 1) & RLDA_M_SECT);

        // End of track?
        if (GET_SECT (unit.currentTrack_) >= RL_NUMSC)
            unit.currentTrack_ &= ~RLDA_M_SECT;                      /* wrap to 0 */
        return;
    }

    if (unit.function_ == RLCS_RNOHDR)
    {
        if (GET_SECT(unit.currentTrack_) >= RL_NUMSC)
        {
            // Wrong cylinder?
            setDone (RLCS_ERR | RLCS_HDE);
            return;
        }

        // Get disk addr
        da = GET_DA(unit.currentTrack_) * RL_NUMWD;

        // Max transfer
        maxwc = (RL_NUMSC - GET_SECT(unit.currentTrack_)) * RL_NUMWD;
    }
    else
    {
        // Bad cyl or sector?
        if (((unit.currentTrack_ & RLDA_CYL) != (rlda & RLDA_CYL)) ||
            (GET_SECT(rlda) >= RL_NUMSC))
        {
            // Wrong cylinder?
            setDone (RLCS_ERR | RLCS_HDE | RLCS_INCMP); 
            return;
        }

        // Get disk addr and max transfer
        da = GET_DA(rlda) * RL_NUMWD;
        maxwc = (RL_NUMSC - GET_SECT(rlda)) * RL_NUMWD;
    }

    // Get mem addr 
    ma = (rlbae << 16) | rlba;                              

    // Get true wc
    wc = 0200000 - rlmpr;

    // track overrun?
    if (wc > maxwc)                                         
        wc = maxwc;

    err = fseek (unit.filePtr_, da * sizeof(int16_t), SEEK_SET);
    
    //if (DEBUG_PRS(rl_dev))
    //    fprintf(sim_deb, ">>RL svc: cyl %d, sect %d, wc %d, maxwc %d, err %d\n",
    //        GET_CYL(rlda), GET_SECT(rlda), wc, maxwc, err);

    if ((unit.function_ >= RLCS_READ) && (err == 0))
    {
        // Read (no hdr)?
        // Read wc * 2 bytes; returned is the number of words read 
        i = fread (rlxb_, sizeof(int16_t), wc, unit.filePtr_);

        // ToDo: Check error
        err = ferror(unit.filePtr_);

        // Clear the part of the buffer not filled by the read
        for (; i < wc; i++)
            rlxb_[i] = 0;

        // Transfer words in buffer
        for (i = 0; i < wc; ++i)
            if (!bus->writeWord (ma, rlxb_[i]))
                rlcs = rlcs | RLCS_ERR | RLCS_NXM;
    }

    else
        if ((unit.function_ == RLCS_WRITE) && (err == 0))
        {    
            CondData<u16> tmpValue;

            // Write?
            for (i = 0; i < wc; ++i)
            {
                tmpValue = bus->read(ma).valueOr(0);
                if (!tmpValue.hasValue())
                {
                    rlcs = rlcs | RLCS_ERR | RLCS_NXM;
                    // Set adj xfer length
                    wc -= i;
                    break;
                }
                rlxb_[i] = tmpValue;
            }

            // Any xfer?
            if (wc)
            {
                // Clear to end of block
                awc = (wc + (RL_NUMWD - 1)) & ~(RL_NUMWD - 1);  
                for (i = wc; i < awc; i++)
                    rlxb_[i] = 0;

                fwrite (rlxb_, sizeof(int16_t), awc, unit.filePtr_);
                err = ferror(unit.filePtr_);
            }
        } 

        else
            if ((unit.function_ == RLCS_WCHK) && (err == 0))
            {
                CondData<u16> comp;

                // Write check?
                i = fread (rlxb_, sizeof(int16_t), wc, unit.filePtr_);
                err = ferror(unit.filePtr_);

                // Fill buffer
                for (; i < wc; i++)                                
                    rlxb_[i] = 0;

                // Save wc
                awc = wc;
                for (wc = 0; (err == 0) && (wc < awc); wc++)
                {
                    // Loop through buffer
                    comp = bus->read (ma).valueOr(0);
                    if (!comp.hasValue())
                    { 
                        rlcs = rlcs | RLCS_ERR | RLCS_NXM;
                        break;
                    }

                    // Check read word with buffer
                    if (comp != rlxb_[wc])
                        rlcs = rlcs | RLCS_ERR | RLCS_CRC;
                } 
            }

    // Complete Write Check, Write, Read, Read no header
    // Final word count */
    rlmpr = (rlmpr + wc) & 0177777;

    // Completed?
    if (rlmpr != 0)
        rlcs |= RLCS_ERR | RLCS_INCMP | RLCS_HDE;

    ma += (wc << 1);                                        /* final byte addr */
    rlbae = (ma >> 16) & RLBAE_IMP;                         /* upper 6b */
    rlba = ma & RLBA_IMP;                                   /* lower 16b */
    rlcs = (rlcs & ~RLCS_MEX) | ((rlbae & RLCS_M_MEX) << RLCS_V_MEX);

    // If we ran off the end of the track, return 40 in rlda, but keep
    // track over a legitimate sector (0)?
    rlda += ((wc + (RL_NUMWD - 1)) / RL_NUMWD);

    // Update head position
    if (unit.function_ == RLCS_RNOHDR)
        unit.currentTrack_ = (unit.currentTrack_ & ~RLDA_M_SECT) |
        ((unit.currentTrack_ + ((wc + (RL_NUMWD - 1)) / RL_NUMWD)) & RLDA_M_SECT);
    else
        unit.currentTrack_ = rlda;
    if (GET_SECT(unit.currentTrack_) >= RL_NUMSC)
        unit.currentTrack_ &= ~RLDA_M_SECT;                          /* wrap to 0 */

    setDone(0);

    if (err != 0)
    {
        perror("RL I/O error");
        clearerr(unit.filePtr_);
    }

    return;
}