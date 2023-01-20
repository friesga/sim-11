#ifndef _RXV21DISK_H_
#define _RXV21DISK_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"

#include <fstream>

// Definition of the type to discriminate the trace records in the template
// classes.
struct RXV21Disk {};

// Definition of the RXV21 commands. An old-style enum s used as type to be
// able to use the function codes from the RX2CS register.
// ToDo: These definitions should be replaced by definitions in rxv21.h (but
// these are still defined as macro's.
enum RXV21DiskCmd
{
	RXV21_FILL     = (00 << 1),
	RXV21_EMPTY    = (01 << 1),
	RXV21_WRITE    = (02 << 1),
	RXV21_READ     = (03 << 1),
	RXV21_SET_MD   = (04 << 1),
	RXV21_STATUS   = (05 << 1),
	RXV21_WRITE_DD = (06 << 1),
	RXV21_READ_ERR = (07 << 1)
};

// Specialization of the TraceRecord for the RXV21Disk record
template <>
class TraceRecord<RXV21Disk>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<RXV21Disk> record);

	RXV21DiskCmd type_;
	u8	drive_;
	u8	density_;
	u16	rx2sa_;
	u16	rx2ta_;

public:
	TraceRecord ();
	TraceRecord (RXV21DiskCmd type, u8 drive, u8 density, u16 rx2sa, u16 rx2ta);
    Magic magic () {return Magic::RX2A;}
};

// Constructors for the TraceRecord<RXV21Disk> type
inline TraceRecord<RXV21Disk>::TraceRecord ()
	:
	type_ {RXV21DiskCmd::RXV21_EMPTY},
	drive_ {0},
	density_ {0},
	rx2sa_ {0},
	rx2ta_ {0}
{}

inline TraceRecord<RXV21Disk>::TraceRecord (RXV21DiskCmd type, u8 drive,
	u8 density, u16 rx2sa, u16 rx2ta)
	:
	type_ {type},
	drive_ {drive},
	density_ {density},
	rx2sa_ {rx2sa},
	rx2ta_ {rx2ta}
{}

#endif // _RXV21DISK_H_