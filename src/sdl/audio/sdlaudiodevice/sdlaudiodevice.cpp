#include "sdlaudiodevice.h"
#include "../sdlaudiospec/sdlaudiospec.h"

#include <stdexcept>

SDLAudioDevice::SDLAudioDevice (const AudioFormat& format,
    SDL_AudioCallback callback, void* userdata)
{
    init (format, callback, userdata);
}

SDLAudioDevice::~SDLAudioDevice ()
{
    SDL_CloseAudioDevice (audioDevice_);
}

void SDLAudioDevice::start ()
{
    SDL_PauseAudioDevice (audioDevice_, 0);
}

void SDLAudioDevice::pause ()
{
    SDL_PauseAudioDevice (audioDevice_, 1);
}

bool SDLAudioDevice::isPlaying () const
{
    return SDL_GetAudioDeviceStatus (audioDevice_) ==
        SDL_AudioStatus::SDL_AUDIO_PLAYING;
}

void SDLAudioDevice::lock ()
{
    SDL_LockAudioDevice (audioDevice_);
}

void SDLAudioDevice::unlock ()
{
    SDL_UnlockAudioDevice (audioDevice_);
}

AudioFormat SDLAudioDevice::audioFormat () const
{
    return SDLAudioSpec {obtainedAudioSpec_};
}

void SDLAudioDevice::init (const AudioFormat& desiredAudioFormat,
    SDL_AudioCallback callback, void* userdata)
{
    SDL_AudioSpec desiredAudioSpec = SDLAudioSpec {desiredAudioFormat};

    desiredAudioSpec.samples = 1024;
    desiredAudioSpec.callback = callback;
    desiredAudioSpec.userdata = userdata;

    audioDevice_ = SDL_OpenAudioDevice (
        nullptr,
        0,
        &desiredAudioSpec,
        &obtainedAudioSpec_,
        SDL_AUDIO_ALLOW_FREQUENCY_CHANGE
    );

    if (audioDevice_ == 0)
        throw std::runtime_error ("SDL_OpenAudioDevice failed: " + string (SDL_GetError ()));

    if (obtainedAudioSpec_.format != desiredAudioSpec.format ||
        obtainedAudioSpec_.channels != desiredAudioSpec.channels)
    {
        SDL_CloseAudioDevice (audioDevice_);
        audioDevice_ = 0;
        throw std::runtime_error ("Unexpected audio format from SDL_OpenAudioDevice");
    }
}
