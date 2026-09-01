#include "sdlaudiostream.h"
#include "../sdlaudiospec/sdlaudiospec.h"

#include <algorithm>
#include <memory>
#include <vector>
#include <stdexcept>
#include <cstring>

using std::memcpy;

// Construct an AudioStream from the given WAV file, convert it to the
// given format and store it in the samples_ vector.
//
SDLAudioStream::SDLAudioStream (const char* filename,
    const AudioFormat& targetSpec)
{
    loadWAVFile (filename, targetSpec);
}

// Construct an AudioStream from the given vector of AudioFrames.
//
// This constructor is added for testing purposes
//
SDLAudioStream::SDLAudioStream (vector<AudioFrame> frames)
    :
    samples_ {frames}
{}

// Construct an AudioStream from the given data, convert it to the given
// format and store it in the samples_ vector.
//
SDLAudioStream::SDLAudioStream (vector<uint8_t>& wavData,
    const AudioFormat& audioFormat)
{
    SDL_IOStream* ioStream = SDL_IOFromConstMem (wavData.data (),
        static_cast<int>(wavData.size ()));

    if (ioStream == nullptr)
        throw std::runtime_error (SDL_GetError ());

    SDL_AudioSpec wavSpec {};
    Uint8* wavBuffer = nullptr;
    Uint32 wavLength = 0;

    if (!SDL_LoadWAV_IO (ioStream, 1, &wavSpec, &wavBuffer, &wavLength))
        throw std::runtime_error (SDL_GetError ());

    WAVBufferPtr wavBufferPtr {wavBuffer, SDL_free};

    convertWAVdata (wavBufferPtr, wavLength, wavSpec, SDLAudioSpec {audioFormat});
}

// This funcion loads the specified WAV file and converts it to the target
// audio format specified by targetSpec
//
void SDLAudioStream::loadWAVFile (const char* filename,
    const AudioFormat& audioFormat)
{
    SDL_AudioSpec wavSpec {};
    SDL_AudioSpec targetSpec = SDLAudioSpec {audioFormat};

    Uint8* rawWavBuffer {};
    Uint32 wavLength {};

    // SDL_LoadWAV will load the the specified WAV file into a raw data buffer,
    // return an SDL_AudioSpec filled with the WAV file's format details and
    // the length of the audio data in bytes. SDL_LoadWAV will allocate memory
    // for the raw data buffer, which must be freed with SDL_FreeWAV when no
    // longer needed.
    if (!SDL_LoadWAV (filename, &wavSpec, &rawWavBuffer, &wavLength))
        throw std::runtime_error ("WAV file " + string (filename) + " could not be loaded");

    // Create a unique_ptr to the WAV data from the raw buffer memory,
    // ensuring the buffer is freed when no longer needed.
    WAVBufferPtr wavBuffer {rawWavBuffer, SDL_free};

    convertWAVdata (wavBuffer, wavLength, wavSpec, targetSpec);
}

bool SDLAudioStream::eos ()
{
    return samples_.empty () || playPosition_ == samples_.size ();
}

void SDLAudioStream::reset ()
{
    playPosition_ = 0;
}

// This function fills the given stream buffer with the given number
// of AudioFrames till the end of the samples_ buffer has been reached
size_t SDLAudioStream::fill (AudioFrame* stream, size_t numberOfFrames)
{
    if (samples_.empty ())
        return 0;

    size_t framesWritten = 0;

    while (framesWritten < numberOfFrames &&
        playPosition_ < samples_.size ())
    {
        stream[framesWritten] = samples_[playPosition_];

        ++playPosition_;
        ++framesWritten;
    }

    return framesWritten;
}

// This function converts the WAV data in the given buffer with the given
// length and in the given format, to data in the samples_ vector with
// the given target format.
//
void SDLAudioStream::convertWAVdata (WAVBufferPtr& wavBuffer, Uint32 bufferSize,
    SDL_AudioSpec fromSpec, SDL_AudioSpec targetSpec)
{
    Uint8* convertedData = nullptr;
    int convertedDataLength = 0;

    Uint8* sourceData = wavBuffer.get ();
    const int sourceDataLength = static_cast<int> (bufferSize);

    if (!SDL_ConvertAudioSamples (&fromSpec, sourceData, sourceDataLength, &targetSpec,
        &convertedData, &convertedDataLength))
    {
        throw std::runtime_error (SDL_GetError ());
    }

    const std::size_t actualFrameCount =
        static_cast<std::size_t>(convertedDataLength) / sizeof (AudioFrame);

    const auto* frames =
        reinterpret_cast<const AudioFrame*>(convertedData);

    samples_.insert (samples_.end (), frames, frames + actualFrameCount);

    SDL_free (convertedData);
}