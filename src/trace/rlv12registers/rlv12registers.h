#ifndef _RLV12REGISTERS_H_
#define _RLV12REGISTERS_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"
#include "../tracefileoutstream/tracefileoutstream.h"
#include "../tracefileinstream/tracefileinstream.h"

#include <fstream>
#include <string>

using std::string;

// Definition of the type to discriminate the trace records in the template
// classes.
struct RLV12Registers {};

// Specialization of the TraceRecord for the RLV12Registers record
template <>
class TraceRecord<RLV12Registers>
{
    friend TracefileOutStream& operator<< (TracefileOutStream& tos, 
        TraceRecord<RLV12Registers> record);
    friend TracefileInStream &operator>> (TracefileInStream &is,
        TraceRecord<RLV12Registers> &record);
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<RLV12Registers> record);

	u16 rlcs_;
    u16 rlba_;
    u16 rlda_;
    u16 rlmpr_;
    u16 rlbae_;
	size_t length_;
    string msg_;

public:
	TraceRecord ();
	TraceRecord (string msg, u16 rlcs, u16 rlba, u16 rlda,
        u16 rlmpr, u16 rlbae);
    Magic magic () {return Magic::RL2A;}

    // Definition of accessors to be used in unit tests
    u16 rlcs () {return rlcs_; }
    u16 rlba () {return rlba_; }
    u16 rlda () {return rlda_; }
    u16 rlmpr () {return rlmpr_; }
    u16 rlbae () {return rlbae_; }
    size_t length () {return length_; }
    string msg () {return msg_; }
};

inline TraceRecord<RLV12Registers>::TraceRecord ()
	:
    rlcs_ {0},
    rlba_ {0},
    rlda_ {0},
    rlmpr_ {0},
    rlbae_ {0},
    length_ {0},
    msg_ {}
{}

inline TraceRecord<RLV12Registers>::TraceRecord (string msg, u16 rlcs,
    u16 rlba, u16 rlda, u16 rlmpr, u16 rlbae)
	:
    rlcs_ {rlcs},
    rlba_ {rlba},
    rlda_ {rlda},
    rlmpr_ {rlmpr},
    rlbae_ {rlbae},
    length_ {msg.length()},
    msg_ {msg}
{}

#endif // _RLV12REGISTERS_H_