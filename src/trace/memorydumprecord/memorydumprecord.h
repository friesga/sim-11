#ifndef _MEMORYDUMPRECORD_H_
#define _MEMORYDUMPRECORD_H_

#include "types.h"
#include "../tracefileoutstream/tracefileoutstream.h"
#include "../tracefileinstream/tracefileinstream.h"

#include <memory>

using std::shared_ptr;

// Definition of the type to discriminate the trace records in the template
// classes.
struct MemoryDumpRecord {};

// Specialization of the TraceRecord for the MemoryDump record
template <>
class TraceRecord<MemoryDumpRecord>
{
    friend TracefileOutStream& operator<< (TracefileOutStream& tos, 
        TraceRecord<MemoryDumpRecord> record);
    friend TracefileInStream &operator>> (TracefileInStream &is,
        TraceRecord<MemoryDumpRecord> &record);
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<MemoryDumpRecord> record);

	u16	address_;
	u16 length_;

    // Definition of a smart pointer to the memory to be written to/
    // read from the tracefile. 
    // 
    // The pointer services two purposes:
    // 1. When writing the tracefile to pass the memory address to dump
    //    from the TraceRecord<MemoryDumpRecord> constructor to the function
    //    operator<< (TracefileOutStream&, ...) writing the record to the
    //    trace file, and
    // 2. When reading a trafecefile to pass the address of the allocated
    //    buffer from the function operator>> (std::istream &is, ...) reading
    //    a record from the trace file, to the function printing the record
    //    operator<< (std::ostream& os, ...).
    // 
    //    The pointer isn't written to the tracefile and therefore has to be
    //    declared as the last data member of the class.
    std::shared_ptr<u8[]> ptr_;

public:
	TraceRecord ();
	TraceRecord (u8 *ptr, u16 address, u16 length);
    Magic magic () {return Magic::BUS1;}

    // Definition of accessors to be used in unit tests
    u16	address () { return address_; }
    u16 length () { return length_; }
    shared_ptr<u8[]> ptr () { return ptr_; }
};

// Default constructor for the MemoryDump record
inline TraceRecord<MemoryDumpRecord>::TraceRecord ()
	:
    address_ {0},
    length_ {0},
    ptr_ {nullptr}
{}

inline TraceRecord<MemoryDumpRecord>::TraceRecord (u8 *ptr, u16 address, u16 length)
	:
    address_ {address},
    length_ {length}
{
    // Assign the raw pointer ptr to the shared pointer with a custom deleter
    // that will not free the memory pointed to by the raw pointer.
    ptr_.reset (ptr, [](u8[]){});
}

// Definition of a suffix operator to be able to express values as kilobytes.
constexpr size_t operator""_KB (unsigned long long const x)
{ 
	return 1024L * x;
}

#endif // _MEMORYDUMPRECORD_H_