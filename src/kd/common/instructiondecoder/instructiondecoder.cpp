#include "instructiondecoder.h"
#include "kd/common/instructions/unused.h"

Instruction InstructionDecoder::decode (u16 instruction)
{
    return decodeGroup_nn_xx_xx (instruction);
}

Instruction InstructionDecoder::decodeGroup_00_00_nn (u16 instruction)
{
    if (instruction <= 07)
        return (*group_00_00_nn [instruction]) (instruction);
        
    return create<Unused> (instruction);
}

Instruction InstructionDecoder::decodeGroup_00_02_nn (u16 instruction)
{
    return (*group_00_02_nn [instruction - 0200]) (instruction);
}

Instruction InstructionDecoder::decodeGroup_07_5n_nx (u16 instruction)
{
    u16 index = (instruction >> 3) - 07500;
    if (index < 010)
        return (*group_07_5n_nx [index]) (instruction);
        
    return create<Unused> (instruction);
}

Instruction InstructionDecoder::decodeGroup_07_nx_xx (u16 instruction)
{
    return (*group_07_nx_xx [(instruction >> 9) - 070]) (instruction);
}

Instruction InstructionDecoder::decodeGroup_10_xx_xx (u16 instruction)
{
    return (*group_10_xx_xx [(instruction >> 6) - 01000]) (instruction);
}

Instruction InstructionDecoder::decodeGroup_00_nn_xx (u16 instruction)
{
    return (*group_00_nn_xx [instruction >> 6]) (instruction);
}

Instruction InstructionDecoder::decodeGroup_nn_xx_xx (u16 instruction)
{
    return (*group_nn_xx_xx [instruction >> 12]) (instruction);
}
