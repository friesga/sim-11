#include "sdlaudiodevice.h"
#include "../sdlaudiospec/sdlaudiospec.h"

#include <stdexcept>

// The audio device is started by default
SDLAudioDevice::SDLAudioDevice (const AudioFormat& format,
    SDL_AudioStreamCallback callback, void* userdata)
{
    init (format, callback, userdata);
    start ();
}

SDLAudioDevice::~SDLAudioDevice ()
{
    // As the stream was created with SDL_OpenAudioDeviceStream, the audio
    // device to which this stream was bound will be closed too.
    SDL_DestroyAudioStream (audioStream_);
}

void SDLAudioDevice::start ()
{
    SDL_ResumeAudioStreamDevice (audioStream_);
}

void SDLAudioDevice::pause ()
{
    SDL_PauseAudioStreamDevice (audioStream_);
}

bool SDLAudioDevice::isPlaying () const
{
    return !SDL_AudioStreamDevicePaused (audioStream_);
}

void SDLAudioDevice::lock ()
{
    SDL_LockAudioStream (audioStream_);
}

void SDLAudioDevice::unlock ()
{
    SDL_UnlockAudioStream (audioStream_);
}

AudioFormat SDLAudioDevice::audioFormat () const
{
    SDL_AudioSpec sourceAudioSpec_ {};
    SDL_AudioSpec destinationAudioSpec_ {};

    SDL_GetAudioStreamFormat (audioStream_, &sourceAudioSpec_, &destinationAudioSpec_);
    return SDLAudioSpec {destinationAudioSpec_};
}

void SDLAudioDevice::init (const AudioFormat& desiredAudioFormat,
    SDL_AudioStreamCallback callback, void* userdata)
{
    SDL_AudioSpec desiredAudioSpec = SDLAudioSpec {desiredAudioFormat};

    audioStream_ = SDL_OpenAudioDeviceStream (
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
        &desiredAudioSpec,
        callback,
        userdata
    );

    if (audioStream_ == NULL)
        throw std::runtime_error ("SDL_OpenAudioDevice failed: " + string (SDL_GetError ()));
}
