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

    int32_t err, wordCount, maxwc;
    int32_t i, da, awc;
    uint32_t memoryAddress;
    RL01_2 &unit = static_cast<RL01_2&> (unitRef);
 
#if 0
    const char* const funcname[] = {
        "NOP", "WCK", "GSTA", "SEEK",
        "RHDR", "WT", "RD", "RNOHDR", "SPECIAL",
    };
#endif //0

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
        switch (unit.driveStatus_ & RLDS_M_STATE)
        {
            // The LOAD state is a little different.  We can stay in LOAD until
            // the user hits the RUN (LOAD) button, at which time we should come
            // here to transition to the next state and begin the startup process.
            case RLDS_LOAD:
                // Load pressed, spinning up 
                if (!(unit.driveStatus_ & RLDS_CVO))
                {
                    unit.driveStatus_ = (unit.driveStatus_ & ~RLDS_M_STATE) | RLDS_SPIN;
                    // Actual time is 45-50 seconds from press to Lock 
                    timer.start (&unit, std::chrono::milliseconds (200 * rl_swait));
                    
                    unit.driveStatus_ &= ~RLDS_HDO;
                    unit.driveStatus_ |= RLDS_BHO;
                }
                break;
                
                // Original RL01 drives would transition to the Brush Cycle, but this
                // was removed in a later ECO.
                
            case RLDS_SPIN:     
                // Spun up, load brushes or heads
                if (unit.rlStatus_ & RlStatus::UNIT_BRUSH)
                {
                    unit.driveStatus_ &= ~RLDS_BHO;
                    unit.driveStatus_ = (unit.driveStatus_ & ~RLDS_M_STATE) | RLDS_BRUSH;
                }
                else
                {
                    unit.driveStatus_ |= RLDS_BHO;
                    unit.driveStatus_ = (unit.driveStatus_ & ~RLDS_M_STATE) | RLDS_HLOAD;
                }
                timer.start (&unit, std::chrono::milliseconds (200 * rl_swait));
                break;

            case RLDS_BRUSH:
                unit.driveStatus_ = (unit.driveStatus_ & ~RLDS_M_STATE) | RLDS_HLOAD;
                unit.driveStatus_ |= RLDS_BHO;
                timer.start (&unit, std::chrono::milliseconds (200 * rl_swait));
                break;

            case RLDS_HLOAD:
                // Heads loaded, seek to home 
                unit.driveStatus_ = (unit.driveStatus_ & ~RLDS_M_STATE) | RLDS_SEEK;
                timer.start (&unit, std::chrono::milliseconds (200 * rl_swait));
                unit.driveStatus_ |= RLDS_BHO | RLDS_HDO;
                unit.currentTrackHeadSector_ = 0;
                break;

            case RLDS_SEEK:
                // Home found, lock on 
                // Enter postion mode
                unit.driveStatus_ = (unit.driveStatus_ & ~RLDS_M_STATE) | RLDS_LOCK;
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
                unit.driveStatus_ = (unit.driveStatus_ & ~RLDS_M_STATE) | RLDS_DOWN;

                // Retract heads
                unit.driveStatus_ &= ~RLDS_HDO;    

                // Actual time is ~30 seconds
                timer.start (&unit, std::chrono::milliseconds (200 * rl_swait));
                break;

            case RLDS_DOWN:
                // OK to open cover
                unit.driveStatus_ = (unit.driveStatus_ & ~RLDS_M_STATE) | RLDS_LOAD;
                unit.driveStatus_ |= RLDS_BHO | RLDS_VCK;
                break;

            default:
                // Can't happen
                break; 
        }
        return;
    }


    // The following condition isn't met as writeWord() only calls this
    // service routine if the unit is attached.
    // Attached?
    if ( !(unit.unitStatus_ & Status::UNIT_ATT))
    {
        // Spin error
        unit.driveStatus_ |= RLDS_SPE;                       

        // Flag error
        setDone (RLCS_ERR | RLCS_INCMP);                
        // ToDo: return IORETURN(rl_stopioe, SCPE_UNATT); ?
        return;
    }

    if ((unit.function_ == RLCS_WRITE) && 
        ((unit.unitStatus_ & Status::UNIT_RO) || 
          unit.rlStatus_ & RlStatus::UNIT_WLK))
    {
        // Write and locked
        unit.driveStatus_ |= RLDS_WGE;                     
        setDone (RLCS_ERR | RLCS_DRE);
        return;
    }


    if (unit.function_ == RLCS_SEEK)
    {
        // Seek?
        // Enter position mode
        // Heads locked on cyl
        unit.driveStatus_ = (unit.driveStatus_ & ~RLDS_M_STATE) | RLDS_LOCK; 
        return;
    }


    if (unit.function_ == RLCS_RHDR)
    {
        // Read header?
        u16 hdr[2];

        hdr[0] = rlmpr = unit.currentTrackHeadSector_ & 0177777;
        hdr[1] = rlmpr1 = 0;
        // Calculate header CRC
        rlmpr2 = calcCRC(2, &hdr[0]);
        setDone (0);

        // Simulate sequential rotation about the current track
        // This functionality supports the Read Without Header Check
        // procedure, refer to EK-RLV12-UG-002, par. 5.8.
        unit.currentTrackHeadSector_ = 
            (unit.currentTrackHeadSector_ & ~RLDA_M_SECT) |
            ((unit.currentTrackHeadSector_ + 1) & RLDA_M_SECT);

        // End of track?
        if (getSector (unit.currentTrackHeadSector_) >= RL_NUMSC)
            unit.currentTrackHeadSector_ &= ~RLDA_M_SECT;                      /* wrap to 0 */
        return;
    }

    // Create RLV12 command containing the required parameters
    std::unique_ptr<RLV12Command> rlv12Command = 
        createCommand (unit.function_, unit.currentTrackHeadSector_, rlda,
            memAddrFromRegs (), 0200000 - rlmpr);

    if (rlv12Command == nullptr)
        // setDone() has already been executed by createCommand()
        return;

    wordCount = rlv12Command->wordCount ();
    memoryAddress = rlv12Command->memoryAddress ();

    // err = fseek (unit.filePtr_, da * sizeof(int16_t), SEEK_SET);
    if (!fseek (unit.filePtr_, rlv12Command->filePosition(), SEEK_SET))
    {
        //if (DEBUG_PRS(rl_dev))
        //    fprintf(sim_deb, ">>RL svc: cyl %d, sect %d, wordCount %d, maxwc %d, err %d\n",
        //        GET_CYL(rlda), GET_SECT(rlda), wordCount, maxwc, err);

        rlv12Command->execute (this, &unit);
    }

    // WordCount and memoryAddress are changed in execute()!
    wordCount = rlv12Command->wordCount ();

    // Complete Write Check, Write, Read, Read no header
    // Calculate the final word count (i.e. the remaining number of
    // words to be transferred).
    rlmpr = (rlmpr + wordCount) & 0177777;

    // If the specified transfer could not be completed indicate an error
    // condition
    if (rlmpr != 0)
        rlcs |= RLCS_ERR | RLCS_INCMP | RLCS_HDE;

    memoryAddress += (wordCount << 1);                                        /* final byte addr */
    
    // Load BAR, CSR and possibly BAE registers with the current address
    memAddrToRegs (memoryAddress);

    // If we ran off the end of the track, return 40 in rlda, but keep
    // track over a legitimate sector (0)?
    rlda += ((wordCount + (RL_NUMWD - 1)) / RL_NUMWD);

    // Update head position
    if (unit.function_ == RLCS_RNOHDR)
        unit.currentTrackHeadSector_ = (unit.currentTrackHeadSector_ & ~RLDA_M_SECT) |
        ((unit.currentTrackHeadSector_ + ((wordCount + (RL_NUMWD - 1)) / RL_NUMWD)) & RLDA_M_SECT);
    else
        unit.currentTrackHeadSector_ = rlda;
    if (getSector (unit.currentTrackHeadSector_) >= RL_NUMSC)
        unit.currentTrackHeadSector_ &= ~RLDA_M_SECT;                          /* wrap to 0 */

    // RLCSR status error bits are set in execute().
    setDone(0);

    /*
    if (err != 0)
    {
        perror("RL I/O error");
        clearerr(unit.filePtr_);
    }
    */

    return;
}