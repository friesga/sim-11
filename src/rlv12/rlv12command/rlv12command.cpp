#include "rlv12command.h"
#include "types.h"

RLV12Command::RLV12Command (u8 function, int32_t diskAddress, BusAddress memoryAddress, 
        u32 wordCount)
    :
    function_ {function},
    diskAddress_ {diskAddress},
    memoryAddress_ {memoryAddress},
    wordCount_ {wordCount}
{}