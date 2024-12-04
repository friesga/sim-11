#ifndef _ABSOLUTE_LOADER_H_
#define _ABSOLUTE_LOADER_H_

#include "types.h"

// Implementation of the absolute loader. The format of absolute loader input
// is that it consists of an arbitrary number of blocks, each of which has the
// following format :
// - Leader (all bytes 0)
// - Start marker (1 byte, value '1')
// - Pad (1 byte, value '0')
// - Low byte count (1 byte)
// - High byte count (1 byte)
// - Low load address (1 byte)
// - High load address (1 byte)
// - Program to be loaded
// - Checksum (1 byte)
//
// The optional leader is because the absolute loader was originally for
// loading programs held on paper tape. The byte count includes the header
// starting with the 'start marker' (i.e. 6 bytes), but does not include the
// checksum byte.
//
// The checksum is the low-order byte of the negation of the sum of all the
// bytes in a block; when all the bytes of a block, including the checksum,
// are added together, the low-order byte of the result should be zero (if the
// block has been read correctly).
// 
// A block of length 6 (i.e.only the header) marks the end of the input. If the
// address is odd, the absolute loader halts; if even, it jumps to the address
// given in the last block.
//
// Source: https://gunkies.org/wiki/PDP-11_Absolute_Loader
//
namespace AbsoluteLoader
{
    u16 loadFile (const char* fileName, u8* memory);
}

#endif // _ABSOLUTE_LOADER_H_