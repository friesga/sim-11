#include "memorydump.h"

// Definition of the overloaded operator>>() function to read MemoryDump
// records from the tracefile. These records contain the fixed members as
// defined in the class (minus the ptr_) plus a variable number (defined by
// record.length_) of bytes for the contents of the memory dump.
TracefileInStream &operator>> (TracefileInStream &is, TraceRecord<MemoryDump> &record)
{
    // Allocate space for the memory dump.
    // Create a shared pointer to a dynamically allocated array of u8 elements
    // with a deleter that will free the array.
    size_t constexpr maxMemorySize = 64_KB;
    record.ptr_ = std::shared_ptr<u8[]> (new u8[maxMemorySize],
        std::default_delete<u8[]> ());

    // Read the fixed part of the record
    is.read (reinterpret_cast<char *> (&record),
        sizeof (record.address_) + sizeof (record.length_));

    // Read the variable length memory from the trace file in
    // the allocated buffer
    is.read (reinterpret_cast<char *> (record.ptr_.get()), record.length_);
    return is;
}