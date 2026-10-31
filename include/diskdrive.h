#ifndef _DISKDRIVE_H_
#define _DISKDRIVE_H_

// Definition of the functions every diskdrive has to support so it can be
// handled by the CartridgeHandler.
// 
class Diskdrive
{
public:
    virtual void unloadCartridge () = 0;
    virtual void loadCartridge (Cartridge) = 0;
    virtual bool cartridgeIsLoaded () = 0;
    virtual bool cartridgeCanBeLoaded () = 0;
    virtual ~Diskdrive () = default;
};

#endif // _DISKDRIVE_H_