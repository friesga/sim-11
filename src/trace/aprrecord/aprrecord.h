#ifndef _KTF11_ARECORD_H_
#define _KTF11_ARECORD_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"
#include "kd/kdf11/ktf11_a/ktf11_a_activepageregisterset.h"

#include <iostream>

// Definition of the type to discriminate the trace records in the template
// classes.
struct APRRecord {};

// Specialization of the TraceRecord for the APRRecord record
template <>
class TraceRecord<APRRecord>
{
public:
	friend std::ostream& operator<< (std::ostream& os, 
		TraceRecord<APRRecord> record);

	TraceRecord ();
	TraceRecord (ActivePageRegisterSet const& aprSet);
    Magic magic () {return Magic::MMU0;}

private:
	ActivePageRegisterSet aprSet_;
};

inline TraceRecord<APRRecord>::TraceRecord ()
	:
	aprSet_ {}
{}

inline TraceRecord<APRRecord>::TraceRecord (ActivePageRegisterSet const & aprSet)
	:
	aprSet_ {aprSet}
{}

#endif // !_KTF11_ARECORD_H_
