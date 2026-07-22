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
    SDL_RWops* rw = SDL_RWFromConstMem (wavData.data (),
        static_cast<int>(wavData.size ()));

    if (rw == nullptr)
        throw std::runtime_error (SDL_GetError ());

    SDL_AudioSpec wavSpec {};
    Uint8* wavBuffer = nullptr;
    Uint32 wavLength = 0;

    if (SDL_LoadWAV_RW (rw, 1, &wavSpec, &wavBuffer, &wavLength) == nullptr)
        throw std::runtime_error (SDL_GetError ());

    WAVBufferPtr wavBufferPtr {wavBuffer, SDL_FreeWAV};

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
    if (SDL_LoadWAV (filename, &wavSpec, &rawWavBuffer, &wavLength) == nullptr)
        throw std::runtime_error ("WAV file " + string (filename) + " could not be loaded");

    // Create a unique_ptr to the WAV data from the raw buffer memory,
    // ensuring the buffer is freed when no longer needed.
    WAVBufferPtr wavBuffer {rawWavBuffer, SDL_FreeWAV};

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

// This functions converts the WAV data in the given buffer with the given
// length and in the given format, to data in the samples_ vector with
// the given target format.
//
void SDLAudioStream::convertWAVdata (WAVBufferPtr& wavBuffer, Uint32 bufferSize,
    SDL_AudioSpec fromSpec, SDL_AudioSpec targetSpec)
{
    // Build an SDL_AudioCVT structure that describes how to convert the audio data
    // from the WAV file format (fromSpec) to the target audio format (targetSpec).
    SDL_AudioCVT cvt {};

    if (SDL_BuildAudioCVT (&cvt,
        fromSpec.format, fromSpec.channels, fromSpec.freq,
        targetSpec.format, targetSpec.channels, targetSpec.freq) < 0)
    {
        throw std::runtime_error ("SDL_BuildAudioCVT failed");
    }

    // Set the length of the audio data to be converted from the length
    // of the WAV data loaded by SDL_LoadWAV.
    // len_mult is the length multiplier for determining the size of the
    // converted data. The audio buffer may need to be larger than either
    // the original data or the converted data. The allocated size of buf
    // should be len * len_mult.
    //
    cvt.len = static_cast<int> (bufferSize);

    const auto requiredSizeInBytes =
        static_cast<size_t> (cvt.len) *
        static_cast<size_t> (cvt.len_mult);

    // Round up the required size in bytes to the nearest multiple of the size
    // of an SDLAudioFrame, and then convert to the required size in frames.
    const std::size_t requiredSizeInFrames =
        (requiredSizeInBytes + sizeof (AudioFrame) - 1) / sizeof (AudioFrame);

    // Resize the samples_ vector to accommodate the converted audio data
    // and set cvt buffer pointer to the raw samples_ data. The cvt buffer
    // pointer has to be set after the resize, because the samples_ raw data
    // pointer will be zero if no data has been allocated yet.
    samples_.resize (requiredSizeInFrames);

    // Use the raw samples_ data pointer as the destination buffer for the
    // audio. The conversion buffer cannot be defined as a vector or stream
    // of AudioFrames as the audio format is not known until after the
    // conversion.
    auto* conversionBuffer =
        reinterpret_cast<Uint8*> (samples_.data ());

    memcpy (conversionBuffer, wavBuffer.get (), cvt.len);

    cvt.buf = conversionBuffer;

    if (SDL_ConvertAudio (&cvt) != 0)
        throw std::runtime_error (SDL_GetError ());

    const std::size_t actualFrameCount =
        static_cast<std::size_t>(cvt.len_cvt) / sizeof (AudioFrame);

    samples_.resize (actualFrameCount);
}