#ifndef _ATTACHFLAGS_H_
#define _ATTACHFLAGS_H_

// Attach options, used in Bitmask<AttachFlags>
enum class AttachFlags
{
    Default,                    // Open existing file read/write
    ReadOnly,                   // Attach read-only
    NewFile,                    // Create new file on attach
    Overwrite,                  // Overwrite possibly existing file
    _                           // Required for Bitmask
};

#endif _ATTACHFLAGS_H_