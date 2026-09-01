#include "sdlaudiospec.h"

#include <stdexcept>

SDLAudioSpec::SDLAudioSpec (const AudioFormat & format)
{
    sdl_AudioSpec_.freq = format.sampleRateHz;
    sdl_AudioSpec_.channels = static_cast<Uint8> (format.numberOfChannels);

    switch (format.sampleFormat)
    {
        case AudioSampleFormat::UInt8:
            sdl_AudioSpec_.format = SDL_AUDIO_U8;
            break;

        case AudioSampleFormat::Int16:
            sdl_AudioSpec_.format = SDL_AUDIO_S16;
            break;

        case AudioSampleFormat::Int32:
            sdl_AudioSpec_.format = SDL_AUDIO_S32;
            break;

        case AudioSampleFormat::Float32:
            sdl_AudioSpec_.format = SDL_AUDIO_F32;
            break;

        default:
            throw std::invalid_argument ("Unsupported audio sample format");
    }
}

SDLAudioSpec::SDLAudioSpec (const SDL_AudioSpec& sdlAudioSpec)
{
    sdl_AudioSpec_ = sdlAudioSpec;
}

SDLAudioSpec::operator SDL_AudioSpec () const
{
    return sdl_AudioSpec_;
}

SDLAudioSpec::operator AudioFormat () const
{
    AudioFormat format {};

    format.sampleRateHz = sdl_AudioSpec_.freq;
    format.numberOfChannels = sdl_AudioSpec_.channels;

    switch (sdl_AudioSpec_.format)
    {
        case SDL_AUDIO_U8:
            format.sampleFormat = AudioSampleFormat::UInt8;
            break;

        case SDL_AUDIO_S16:
            format.sampleFormat = AudioSampleFormat::Int16;
            break;

        case SDL_AUDIO_S32:
            format.sampleFormat = AudioSampleFormat::Int32;
            break;

        case SDL_AUDIO_F32:
            format.sampleFormat = AudioSampleFormat::Float32;
            break;

        default:
            throw std::invalid_argument ("Unsupported SDL audio sample format");
    }
    return format;
}