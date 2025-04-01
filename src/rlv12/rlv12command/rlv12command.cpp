#include "rlv12command.h"
#include "types.h"

RLV12Command::RLV12Command (u16 unit, u8 function, int32_t diskAddress,
    BusAddress memoryAddress, u32 wordCount)
    :
    unit_ {unit},
    function_ {function},
    diskAddress_ {diskAddress},
    memoryAddress_ {memoryAddress},
    wordCount_ {wordCount}
{}