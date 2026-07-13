#ifndef _SDLAUDIODEVICE_H_
#define _SDLAUDIODEVICE_H_

#include "../include/audio.h"

#include <SDL.h>

class SDLAudioDevice
{
public:
    explicit SDLAudioDevice (const AudioFormat& format,
        SDL_AudioCallback callback, void *userdata);
    ~SDLAudioDevice ();

    SDLAudioDevice (const SDLAudioDevice&) = delete;
    SDLAudioDevice& operator=(const SDLAudioDevice&) = delete;

    void start ();
    void pause ();
    bool isPlaying () const;
    void lock ();
    void unlock ();
    AudioFormat audioFormat () const;

private:
    SDL_AudioDeviceID audioDevice_ {0};
    SDL_AudioSpec obtainedAudioSpec_ {};

    void init (const AudioFormat& desiredAudioFormat,
        SDL_AudioCallback callback, void *userdata);
};


#endif // _SDLAUDIODEVICE_H_