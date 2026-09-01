#ifndef _SDLAUDIOSPEC_H_
#define _SDLAUDIOSPEC_H_

#include "../include/audio.h"

#include <SDL3/SDL.h>

// This class is a wrapper around SDL_AudioSpec and converts a AudioFormat
// to an SDL_AudioSpec and vice versa.
class SDLAudioSpec
{
public:
    SDLAudioSpec (const AudioFormat& format);
    SDLAudioSpec (const SDL_AudioSpec& sdlAudioSpec);
    operator SDL_AudioSpec () const;
    operator AudioFormat () const;

private:
    SDL_AudioSpec sdl_AudioSpec_ {};
};


#endif // _SDLAUDIOSPEC_H_