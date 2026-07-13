#ifndef _SDLAUDIODEVICELOCK_H_
#define _SDLAUDIODEVICELOCK_H_

#include "../sdlaudiodevice/sdlaudiodevice.h"

class SDLAudioDeviceLock
{
public:
    explicit SDLAudioDeviceLock (SDLAudioDevice& device);
    ~SDLAudioDeviceLock ();

    SDLAudioDeviceLock (const SDLAudioDeviceLock&) = delete;
    SDLAudioDeviceLock& operator=(const SDLAudioDeviceLock&) = delete;

private:
    SDLAudioDevice& device_;
};

#endif // _SDLAUDIODEVICELOCK_H_