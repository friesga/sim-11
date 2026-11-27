#ifndef _DISKDRIVE_H_
#define _DISKDRIVE_H_

#include <string>

// Definition of the functions every diskdrive has to support so it can be
// handled by the CartridgeHandler.
// 
class DiskDrive
{
public:
    enum class State
    {
        NoPower,
        NoCartridgeLoaded,
        CartridgeLoaded_SpindleStopped,
        CartridgeLoaded_DriveSpunUp
    };

    virtual bool unloadCartridge () = 0;
    virtual bool loadCartridge (std::string fileName) = 0;
    virtual State driveState () = 0;
    virtual bool requiresConfirmation () = 0;
    virtual std::string cartridgeFileName () const = 0;
    virtual ~DiskDrive () = default;
};

#endif // _DISKDRIVE_H_