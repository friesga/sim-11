#ifndef _SDLAUDIODEVICE_H_
#define _SDLAUDIODEVICE_H_

#include "../include/audio.h"

#include <SDL3/SDL.h>

class SDLAudioDevice
{
public:
    explicit SDLAudioDevice (const AudioFormat& format,
        SDL_AudioStreamCallback callback, void *userdata);
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
    SDL_AudioStream* audioStream_ {NULL};

    void init (const AudioFormat& desiredAudioFormat,
        SDL_AudioStreamCallback callback, void *userdata);
};


#endif // _SDLAUDIODEVICE_H_