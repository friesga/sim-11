#ifndef _UNIT_H_
#define _UNIT_H_

#include "device.h"
#include "statuscodes.h"
#include "types.h"

#include <cstdio>
#include <string>

// Unit flags
// ToDo: Replace Unit flags by enums or bitset
#define UNIT_ATTABLE    000001                          /* attachable */
#define UNIT_RO         000002                          /* read only */
#define UNIT_FIX        000004                          /* fixed capacity */
#define UNIT_SEQ        000010                          /* sequential */
#define UNIT_ATT        000020                          /* attached */
#define UNIT_BINK       000040                          /* K = power of 2 */
#define UNIT_BUFABLE    000100                          /* bufferable */
#define UNIT_MUSTBUF    000200                          /* must buffer */
#define UNIT_BUF        000400                          /* buffered */
#define UNIT_ROABLE     001000                          /* read only ok */
#define UNIT_DISABLE    002000                          /* disable-able */
#define UNIT_DIS        004000                          /* disabled */
#define UNIT_RAW        010000                          /* raw mode */
#define UNIT_TEXT       020000                          /* text mode */
#define UNIT_IDLE       040000                          /* idle eligible */

// Unit dynamic flags (dynflags) (from simh 4.0)
// These flags are only set dynamically
// ToDo: Replace Unit dynamic flags by enums or bitset
#define UNIT_ATTMULT    000001                          /* allow multiple ATT cmd */
#define UNIT_PIPE       000002                          /* file is a pipe */
#define UNIT_EXTEND     000004                          /* extended SIMH tape format is enabled */


// Definition of an abstract base class for the units of a device
// ToDo: Rename filePtr to something more meaningful
class Unit
{
    Device *owningDevice_;      // Pointer to the controller
    std::string fileName_;      // Name of attached file
    u16 *fileBuffer_;           // Memory pointer for buffered I/O
    size_t hwmark_;             // High water mark
    int32_t position_;          // File position
    u32 dynflags_;              // Dynamic flags

protected:
    FILE *filePtr_;             // The disk file
    u32 capacity_;              // Drive capacity in words
    u32 flags_;                 // Bit flags
    
    // Helper functions for the concrete units
    StatusCode attach_unit (std::string fileName);
    StatusCode createBadBlockTable (int32_t sec, int32_t wds);

public:
    // Constructor
    Unit (Device *owningDevice);

    virtual StatusCode attach (std::string fileName) = 0;
};

#endif // !_UNIT_H_
