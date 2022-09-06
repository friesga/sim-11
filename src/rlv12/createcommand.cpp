#include "rlv12command/rlv12commandfactory.h"
#include "rlv12/rlv12.h"

std::unique_ptr<RLV12Command> 
    RLV12::createCommand (int32_t function, 
        int32_t currentTrackHeadSector, int32_t newTrackHeadSector,
        int32_t memoryAddress, int32_t wordCount)
{
    std::unique_ptr<RLV12Command> rlv12Command;

    switch (function)
    {
        case RLCS_WCHK:
             rlv12Command = CommandFactory<RLV12WriteCheckCmd>::create 
                (currentTrackHeadSector, newTrackHeadSector, 
                 memoryAddress, wordCount);
            break;

        case RLCS_SEEK:
            // ToDo: Implement for Seek command
            break;

        case RLCS_RHDR:
            // ToDo: Correct Read Header functionality
            rlv12Command = CommandFactory<RLV12ReadHeaderCmd>::create 
                (currentTrackHeadSector, newTrackHeadSector,
                 memoryAddress, wordCount);
            break;

        case RLCS_WRITE:
            rlv12Command = CommandFactory<RLV12WriteCmd>::create 
                (currentTrackHeadSector, newTrackHeadSector,
                 memoryAddress, wordCount);
            break;

        case RLCS_READ:
            rlv12Command = CommandFactory<RLV12ReadCmd>::create 
                (currentTrackHeadSector, newTrackHeadSector,
                 memoryAddress, wordCount);
            break;

        case RLCS_RNOHDR:
            if ((rlv12Command = 
                CommandFactory<RLV12ReadNoHeaderCmd>::create 
                    (currentTrackHeadSector, newTrackHeadSector, 
                     memoryAddress, wordCount)) == nullptr)
            {
                // This RLCSR error status deviates from the status for the 
                // other commands. If they were equal the setDone() call could
                // be combined with the call for the other commands.
                setDone (RLCS_ERR | RLCS_HDE);
                return {};
            }
            break;

        default:
            return {};
    }

    // Common return for create calls 
    if (rlv12Command == nullptr)
    {
        setDone (RLCS_ERR | RLCS_HDE | RLCS_INCMP); 
        return {};
    }

    return rlv12Command;
}
