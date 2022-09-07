#include "rlv12command/rlv12commandfactory.h"
#include "rlv12/rlv12.h"

std::unique_ptr<RLV12Command> 
    RLV12::createCommand (int32_t function, 
        int32_t currentDiskAddress, int32_t newDiskAddress,
        int32_t memoryAddress, int32_t wordCount)
{
    std::unique_ptr<RLV12Command> rlv12Command;

    switch (function)
    {
        case RLCS_WCHK:
             rlv12Command = CommandFactory<RLV12WriteCheckCmd>::create 
                (currentDiskAddress, newDiskAddress, 
                 memoryAddress, wordCount);
            break;

        case RLCS_SEEK:
            rlv12Command = CommandFactory<RLV12SeekCmd>::create 
                (currentDiskAddress, newDiskAddress,
                 memoryAddress, wordCount);
            break;

        case RLCS_RHDR:
            rlv12Command = CommandFactory<RLV12ReadHeaderCmd>::create 
                (currentDiskAddress, newDiskAddress,
                 memoryAddress, wordCount);
            break;

        case RLCS_WRITE:
            rlv12Command = CommandFactory<RLV12WriteCmd>::create 
                (currentDiskAddress, newDiskAddress,
                 memoryAddress, wordCount);
            break;

        case RLCS_READ:
            rlv12Command = CommandFactory<RLV12ReadCmd>::create 
                (currentDiskAddress, newDiskAddress,
                 memoryAddress, wordCount);
            break;

        case RLCS_RNOHDR:
            if ((rlv12Command = 
                CommandFactory<RLV12ReadNoHeaderCmd>::create 
                    (currentDiskAddress, newDiskAddress, 
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
