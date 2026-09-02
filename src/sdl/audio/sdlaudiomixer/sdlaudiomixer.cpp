#include "sdlaudiomixer.h"
#include "../sdlaudiodevicelock/sdlauiodevicelock.h"

#include <algorithm>

using std::make_unique;
using std::max;
using std::lock_guard;

// These actions require locking of the audio device. This has to accomplished
// by the caller.
//
void SDLAudioMixer::fill (SDL_AudioStream* audioStream,
    size_t numberOfFrames)
{
    // Make sure the stream buffer has enough capacity to accomodate the
    // requested number of frames.
    streamBuffer_.resize (numberOfFrames);

    // First fill the stream buffer with silence so the channel audio can be
    // added to the AudioFrames in the stream
    std::fill (streamBuffer_.begin (), streamBuffer_.end (), AudioFrame {0.0f, 0.0f});

    size_t framesFilled = mixChannels (streamBuffer_.data (), numberOfFrames);

    adjustVolume (streamBuffer_.data (), framesFilled);

    // Now the filled stream buffer can be written to the audio device
    SDL_PutAudioStreamData (audioStream, streamBuffer_.data (),
        framesFilled * sizeof (AudioFrame));
}

size_t SDLAudioMixer::mixChannels (AudioFrame* stream, size_t numberOfFrames)
{
    size_t numFramesInStream {0};

    channelBuffer_.resize (numberOfFrames);

    // For every available channel read the contents of that channel into
    // a buffer and add that channel into the mix in the output stream.
    // Keep track of the maximum number of AudioFrames put into the stream.
    //
    lock_guard<mutex> lock (audioPlayersMutex_);

    for (AudioPlayer* audioPlayer : audioPlayers_)
    {
        // At this point possibly the channelBuffer should be cleared, but
        // as we keep track of the number of frames filled that should not
        // be necessary.
        // 
        // Disclaimer: the following three statements could be merged into
        // one, thereby removing the necessity for a intermediate variable
        // but this would obfuscate the intention of the code.
        //
        const size_t numFramesInChannel =  audioPlayer->fill (channelBuffer_.data (),
            numberOfFrames);

        mixInto (channelBuffer_.data (), stream, numFramesInChannel);

        numFramesInStream = max (numFramesInStream, numFramesInChannel);
    }

    return numFramesInStream;
}

void SDLAudioMixer::adjustVolume (AudioFrame* stream, size_t numberOfFrames)
{
    for (size_t frameIndex = 0; frameIndex < numberOfFrames; ++frameIndex)
        stream[frameIndex] = (stream[frameIndex] * volume_).clampVolume ();
}

void SDLAudioMixer::createChannel (AudioPlayer* audioPlayer)
{
    lock_guard<mutex> lock (audioPlayersMutex_);
    audioPlayers_.push_back (audioPlayer);
}

size_t SDLAudioMixer::mixInto (const AudioFrame* source, AudioFrame* destination,
    size_t numberOfFrames)
{
    for (size_t index = 0; index < numberOfFrames; ++index)
        destination[index] += source[index];

    return numberOfFrames;
}

void SDLAudioMixer::setOutputVolume (float volume)
{
    volume_ = volume;
}

float SDLAudioMixer::volume ()
{
    return volume_;
}


