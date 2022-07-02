#ifndef _SIM_FIO_H_
#define _SIM_FIO_H_

/* OS-independent, endian independent binary I/O package

   For consistency, all binary data read and written by the simulator
   is stored in little endian data order.  That is, in a multi-byte
   data item, the bytes are written out right to left, low order byte
   to high order byte.  On a big endian host, data is read and written
   from high byte to low byte.  Consequently, data written on a little
   endian system must be byte reversed to be usable on a big endian
   system, and vice versa.

   These routines are analogs of the standard C runtime routines
   fread and fwrite.  If the host is little endian, or the data items
   are size char, then the calls are passed directly to fread or
   fwrite.  Otherwise, these routines perform the necessary byte swaps.
   The function sim_fread swaps in place, sim_fwrite uses an intermediate
   buffer.
*/

#include "types.h"

#include <cstdio>

// We assume all supported systems provide large file support
typedef int64_t        t_offset;

// External references
extern t_offset sim_ftell (FILE *st);
extern int sim_fseek (FILE *st, t_offset offset, int whence);
extern t_offset sim_fsize (FILE* fp);
extern size_t sim_fwrite(void* bptr, size_t size, size_t count, FILE* fptr);

#endif // !_SIM_FIO_H_


