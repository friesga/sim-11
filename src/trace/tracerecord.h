#ifndef _TRACERECORD_H_
#define _TRACERECORD_H_

// A trace file comprises a file header (to recognize it as a valid trace
// file), followed by a series of TraceRecords. Every TraceRecord comprises
// of a record header (TraceRecord<RecordHeader> and the records contents
// (Tracerecord<T>).
// 
// Every TraceRecord class needs three operators:
// 1. An operator<< to write records to the tracefile,
// 2. An operator>> to read these records back from the tracefile,
// 3. An operator<< to print the records in readable format
//    to the given stream.
//
// Two variants of records exist:
// 1. Records with a fixed number of members, as defined in the class,
// 2. Records with these fixed number of members, plus an additional
//    number of words as defined by the record length.
//
// Records of type 1 are written and read by default template functions.
// For type 2 records the header file for that type has to contain specialized
// versions of these functions. Every record requires a function to print
// the contents.

// Definition of the TraceRecord class. The class will be specialized for
// every possible trace record type. To implement this we use templates (i.e.
// static polymorphism) as using dynamic polymorphism leads to the presence
// of vtables in the objects with the consequence that the object data to
// be written to the tracefile is clobbered with the vtable.
template <typename T>
class TraceRecord
{};

#endif // !_TRACERECORD_H_
