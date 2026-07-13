#include "sdlaudiospec.h"

#include <stdexcept>

SDLAudioSpec::SDLAudioSpec (const AudioFormat & format)
{
    sdl_AudioSpec_.freq = format.sampleRateHz;
    sdl_AudioSpec_.channels = static_cast<Uint8> (format.numberOfChannels);

    switch (format.sampleFormat)
    {
        case AudioSampleFormat::UInt8:
            sdl_AudioSpec_.format = AUDIO_U8;
            break;

        case AudioSampleFormat::Int16:
            sdl_AudioSpec_.format = AUDIO_S16SYS;
            break;

        case AudioSampleFormat::Int32:
            sdl_AudioSpec_.format = AUDIO_S32SYS;
            break;

        case AudioSampleFormat::Float32:
            sdl_AudioSpec_.format = AUDIO_F32SYS;
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
        case AUDIO_U8:
            format.sampleFormat = AudioSampleFormat::UInt8;
            break;

        case AUDIO_S16SYS:
            format.sampleFormat = AudioSampleFormat::Int16;
            break;

        case AUDIO_S32SYS:
            format.sampleFormat = AudioSampleFormat::Int32;
            break;

        case AUDIO_F32SYS:
            format.sampleFormat = AudioSampleFormat::Float32;
            break;

        default:
            throw std::invalid_argument ("Unsupported SDL audio sample format");
    }
    return format;
}