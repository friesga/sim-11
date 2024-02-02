#ifndef _KTF11_ARECORD_H_
#define _KTF11_ARECORD_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"
#include "kd/kdf11/kdf11_a/ktf11_a/ktf11_a_activepageregisterset.h"

#include <iostream>

// Definition of the type to discriminate the trace records in the template
// classes.
struct KTF11_ARecord {};

// Specialization of the TraceRecord for the KTF11_ARecord record
template <>
class TraceRecord<KTF11_ARecord>
{
public:
	friend std::ostream& operator<< (std::ostream& os, 
		TraceRecord<KTF11_ARecord> record);

	TraceRecord ();
	TraceRecord (ActivePageRegisterSet const& aprSet);
    Magic magic () {return Magic::MMU0;}

private:
	ActivePageRegisterSet aprSet_;
};

inline TraceRecord<KTF11_ARecord>::TraceRecord ()
	:
	aprSet_ {}
{}

inline TraceRecord<KTF11_ARecord>::TraceRecord (ActivePageRegisterSet const & aprSet)
	:
	aprSet_ {aprSet}
{}

#endif // !_KTF11_ARECORD_H_
